#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TThingyTable.h"
#include "util/TFileManip.h"
#include "util/TStringSearch.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "yuna3/Yuna3TranslationSheet.h"
#include "yuna3/Yuna3Script.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Psx;

const static int textBlockBaseSector = 0x0;
const static int textBlockSize = 0x8000;
const static int numTextBlocks = 130;
//const static int numBattleBlocks = 26;
//const static int battleSectorBase = 0x206;
//const static int battleBlockSize = 0x34;
const static int sectorSize = 0x800;

const static int wavAnalysisNoiseStartThreshold = 0x3F;
const static int wavAnalysisNoiseStopThreshold = 0x0;

struct FreeSpaceSpec {
  int pos;
  int size;
};

string as3bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 3) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as1bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 1) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

string as2bHexLiteral(int num) {
  return "<$" + as2bHex(num) + ">";
}

//const static unsigned int mainExeLoadAddr = 0x8000F800;
//const static unsigned int mainExeBaseAddr = 0x80010000;
//const static unsigned int mapDataOffsetTableAddr = 0x8012335C;
//const static unsigned int numMaps = 0x46;

//TThingyTable tableScript;
//TThingyTable tableFixed;
//TThingyTable tableSjis;
//TThingyTable tableEnd;

TThingyTable tableSjisUtf8;

void generateAnalyzedBoxString(
    std::string& content, std::string& prefix, std::string& suffix,
    bool noDefaultQuotes = false) {
  // scan first line of content for sjis open quote (81 75).
  // if one exists, and is not at the start of the line,
  // take whatever precedes it as the nametag and turn it
  // into a prefix.
  // also, we want to wrap all boxes of spoken text in quotes,
  // and this game makes the convenient stylistic choice of
  // having almost no narration, so we just assume everything
  // will be quoted in the script dump.
  // the handful of lines that aren't will be manually edited later.
  
//  std::cerr << "starting: " << content << std::endl;
  
  // default prefix = open quote
//  prefix = "{";
  
  // check if nametag prefix exists
  bool found = false;
  // fucking unsigned sizes!!
  for (int i = 0; i < (int)(content.size() - 1); i++) {
    unsigned char next = content[i];
    if (next >= 0x80) {
      unsigned char nextnext = content[i + 1];
      
      if ((next == 0x81) && (nextnext == 0x75)) {
        // sjis open quote on first line:
        // turn this into the prefix
        prefix += content.substr(0, i + 2);
        content = content.substr(i + 2, std::string::npos);
        found = true;
        break;
      }
      
      // 2-byte sequence: skip second char
      ++i;
    }
    else if ((char)next == '\n') {
      // we're only looking at the first line
      break;
    }
  }
  
/*  if (!found && !noDefaultQuotes) {
    // default prefix = open quote
    prefix += "{";
  }
  
  // suffix = close quote
  if (!noDefaultQuotes) {
    suffix += "}";
  }*/
  
  // yuna 3 puts a quote at the start of all spoken dialogue,
  // with continuations of dialogue by the same character omitting
  // their name.
  // this works out more nicely for us than in yuna 2,
  // where continuted dialogue had no quote at all.
  if (found) {
    suffix += "}";
  }
  
//  std::cerr << "done: " << content << std::endl;
}

std::string toUtf8(std::string str) {
  // convert from SJIS to UTF8
  
  TBufStream conv;
  conv.writeString(str);
  conv.seek(0);
  
  std::string newStr;
  while (!conv.eof()) {
    if (conv.peek() == '\x0A') {
      newStr += conv.get();
    }
    else if (conv.peek() == '\x09') {
      // linebreaks are padded out with char 0x09 in a very arbitrary
      // and seemingly completely pointless way.
      // ignore for output.
      conv.get();
    }
/*    else if (conv.peek() == '[') {
      std::string name;
      while (!conv.eof()) {
        char next = conv.get();
        name += next;
        if (next == ']') break;
      }
      newStr += name;
    } */
    else {
      TThingyTable::MatchResult result = tableSjisUtf8.matchId(conv);
      
      if (result.id == -1) {
        throw TGenericException(T_SRCANDLINE,
                                "toUtf8()",
                                std::string("bad input string: ")
                                + str);
      }
      
      newStr += tableSjisUtf8.getEntry(result.id);
    }
  }
  
  return newStr;
}

class Yuna3GenericString {
public:
  Yuna3GenericString()
      // needs to not be initialized to -1
      // see Yuna3ScriptReader::flushActiveScript()
    : offset(0),
      size(0),
      mayNotExist(false),
      doBoxAnalysis(false) { }
  
  enum Type {
    type_none,
    type_string,
    type_mapString,
    type_setRegion,
    type_setMap,
    type_setNotCompressible,
    type_addOverwrite,
    type_addFreeSpace,
    type_genericLine,
    type_comment,
    type_marker
  };
  
  Type type;
  
  std::string content;
  std::string prefixBase;
  std::string suffixBase;
  int offset;
  int size;
  bool mayNotExist;
  bool doBoxAnalysis;
  
  std::string idOverride;
  
  int scriptRefStart;
  int scriptRefEnd;
  int scriptRefCode;
  
  std::string regionId;
  
  int mapMainId;
  int mapSubId;
  
  bool notCompressible;
  
  std::vector<int> pointerRefs;
//  int pointerBaseAddr;

  // fuck this
  std::vector<FreeSpaceSpec> freeSpaces;

  std::string translationPlaceholder;
  
  std::vector<int> overwriteAddresses;
  std::vector<int> extraIds;
  std::vector<std::string> genericLines;
  
  std::map<std::string, std::string> propertyMap;
  
protected:
  
};

typedef std::vector<Yuna3GenericString> Yuna3GenericStringCollection;

class Yuna3GenericStringSet {
public:
    
  Yuna3GenericStringCollection strings;
  
  static Yuna3GenericString readString(TStream& src, const TThingyTable& table,
                              int offset) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_string;
    result.offset = offset;
    
    src.seek(offset);
    while (!src.eof()) {
      if (src.peek() == 0x00) {
        src.get();
        result.size = src.tell() - offset;
        return result;
      }
      
      TThingyTable::MatchResult matchCheck
        = table.matchId(src);
      if (matchCheck.id == -1) break;
      
      std::string newStr = table.getEntry(matchCheck.id);
      result.content += newStr;
      
      // HACK
      if (newStr.compare("\\n") == 0) result.content += "\n";
    }
    
    throw TGenericException(T_SRCANDLINE,
                            "Yuna3GenericStringSet::readString()",
                            std::string("bad string at ")
                            + TStringConversion::intToString(offset));
  }
  
  void addString(TStream& src, const TThingyTable& table,
                 int offset) {
    Yuna3GenericString result = readString(src, table, offset);
    strings.push_back(result);
  }
  
  void addRawString(std::string content, int offset, int size) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_string;
    result.content = content;
    result.offset = offset;
    result.size = size;
    strings.push_back(result);
  }
  
  void addOverwriteString(TStream& src, const TThingyTable& table,
                 int offset) {
    Yuna3GenericString result = readString(src, table, offset);
    result.overwriteAddresses.push_back(offset);
    strings.push_back(result);
  }
  
  void addMarker(std::string content) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_marker;
    result.content = content;
    strings.push_back(result);
  }
  
  void addPointerTableString(TStream& src, const TThingyTable& table,
                             int offset, int pointerOffset) {
    // check if string already exists, and add pointer ref if so
    for (unsigned int i = 0; i < strings.size(); i++) {
      Yuna3GenericString& checkStr = strings[i];
      // mapStrings need not apply
      if (checkStr.type == Yuna3GenericString::type_string) {
        if (checkStr.offset == offset) {
          checkStr.pointerRefs.push_back(pointerOffset);
          return;
        }
      }
    }
    
    // new string needed
    Yuna3GenericString result = readString(src, table, offset);
    result.pointerRefs.push_back(pointerOffset);
    strings.push_back(result);
  }
  
  void addComment(std::string comment) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_comment;
    result.content = comment;
    strings.push_back(result);
  }
  
  void addSetNotCompressible(bool notCompressible) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_setNotCompressible;
    result.notCompressible = notCompressible;
    strings.push_back(result);
  }
  
  void addAddOverwrite(int offset) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_addOverwrite;
    result.offset = offset;
    strings.push_back(result);
  }
  
  void addAddFreeSpace(int offset, int size) {
    Yuna3GenericString result;
    result.type = Yuna3GenericString::type_addFreeSpace;
    result.offset = offset;
    result.size = size;
    strings.push_back(result);
  }
  
  void addSetRegion(std::string regionId) {
    Yuna3GenericString str;
    str.type = Yuna3GenericString::type_setRegion;
    str.regionId = regionId;
    strings.push_back(str);
  }
  
  void addGenericLine(std::string content) {
    Yuna3GenericString str;
    str.type = Yuna3GenericString::type_genericLine;
    str.content = content;
    strings.push_back(str);
  }
  
  void addPlaceholderString(std::string id) {
    Yuna3GenericString str;
    str.type = Yuna3GenericString::type_string;
    str.idOverride = id;
    str.offset = 0;
    str.size = 0;
    strings.push_back(str);
  }
  
  void addPlaceholderStrings(std::string baseId, int count) {
    for (int i = 0; i < count; i++) {
      std::string id = baseId + "-" + TStringConversion::intToString(i);
      addPlaceholderString(id);
    }
  }
  
  void addPossiblePlaceholderString(std::string id,
                                    std::string prefix = "",
                                    std::string suffix = "") {
    Yuna3GenericString str;
    str.type = Yuna3GenericString::type_string;
    str.idOverride = id;
    str.offset = 0;
    str.size = 0;
    str.prefixBase = prefix;
    str.suffixBase = suffix;
    str.mayNotExist = true;
    strings.push_back(str);
  }
  
  void addPossiblePlaceholderStrings(std::string baseId, int count) {
    for (int i = 0; i < count; i++) {
      std::string id = baseId + "-" + TStringConversion::intToString(i);
      addPossiblePlaceholderString(id);
    }
  }
  
  void exportToSheet(
      Yuna3TranslationSheet& dst,
      std::ostream& ofs,
      std::string idPrefix) const {
    int strNum = 0;
    for (unsigned int i = 0; i < strings.size(); i++) {
      const Yuna3GenericString& item = strings[i];
      
      if ((item.type == Yuna3GenericString::type_string)
          || (item.type == Yuna3GenericString::type_mapString)) {
        std::string idString = idPrefix
//          + TStringConversion::intToString(strNum)
//          + "-"
          + TStringConversion::intToString(strings[i].offset,
              TStringConversion::baseHex);
        if (!item.idOverride.empty()) idString = item.idOverride;
        
//        std::cerr << idString << endl;
        
        std::string content = item.content;
        std::string prefix = "";
        std::string suffix = "";
        
        if (item.doBoxAnalysis) {
          generateAnalyzedBoxString(content, prefix, suffix);
        }
        
        prefix = item.prefixBase + prefix;
        suffix = item.suffixBase + suffix;
    
//    std::cerr << content << std::endl;
        
        content = toUtf8(content);
        prefix = toUtf8(prefix);
        suffix = toUtf8(suffix);
        
//        std::cerr << content << std::endl;
        
        dst.addStringEntry(
          idString, content, prefix, suffix, item.translationPlaceholder);
        
        ofs << "#STARTSTRING("
          << "\"" << idString << "\""
//          << ", "
//          << TStringConversion::intToString(item.offset,
//              TStringConversion::baseHex)
//          << ", "
//          << TStringConversion::intToString(item.size,
//              TStringConversion::baseHex)
          << ")" << endl;
      
      if (item.offset != -1) {
        ofs << "#SETORIGINALPOS("
          << TStringConversion::intToString(item.offset,
              TStringConversion::baseHex)
          << ", "
          << TStringConversion::intToString(item.size,
              TStringConversion::baseHex)
          << ")" << std::endl;
      }
      
        for (const auto& propertyPair: item.propertyMap) {
          ofs << "#SETSTRINGPROPERTY("
            << "\"" << propertyPair.first << "\""
            << ", \"" << propertyPair.second << "\""
            << ")"
            << std::endl;
        }
        
        if (item.type == Yuna3GenericString::type_mapString) {
          ofs << "#SETSCRIPTREF("
            << TStringConversion::intToString(item.scriptRefStart,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.scriptRefEnd,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.scriptRefCode,
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.freeSpaces.size(); i++) {
          ofs << "#ADDFREESPACE("
            << TStringConversion::intToString(item.freeSpaces[i].pos,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.freeSpaces[i].size,
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.pointerRefs.size(); i++) {
          ofs << "#ADDPOINTERREF("
            << TStringConversion::intToString(item.pointerRefs[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.overwriteAddresses.size(); i++) {
          ofs << "#ADDOVERWRITE("
            << TStringConversion::intToString(item.overwriteAddresses[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.extraIds.size(); i++) {
          ofs << "#ADDEXTRAID("
            << TStringConversion::intToString(item.extraIds[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.genericLines.size(); i++) {
          ofs << item.genericLines[i] << std::endl;
        }
        
//        ofs << "#IMPORT(\"" << idString << "\")" << endl;
        if (item.mayNotExist)
          ofs << "#IMPORTIFEXISTS(\"";
        else
          ofs << "#IMPORT(\"";
        
        ofs << idString << "\")" << endl;
        
        ofs << "#ENDSTRING()" << endl;
        ofs << endl;
        
        ++strNum;
      }
      else if (item.type == Yuna3GenericString::type_setRegion) {
        ofs << "#STARTREGION(\""
          << item.regionId
          << "\")" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_setMap) {
        ofs << "#SETMAP("
          << item.mapMainId
          << ", "
          << item.mapSubId
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_setNotCompressible) {
        ofs << "#SETNOTCOMPRESSIBLE("
          << (item.notCompressible ? 1 : 0)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_addOverwrite) {
        ofs << "#ADDOVERWRITE("
          << TStringConversion::intToString(item.offset,
            TStringConversion::baseHex)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_addFreeSpace) {
        ofs << "#ADDFREESPACE("
          << TStringConversion::intToString(item.offset,
            TStringConversion::baseHex)
          << ", "
          << TStringConversion::intToString(item.size,
            TStringConversion::baseHex)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_genericLine) {
        ofs << item.content << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_comment) {
        dst.addCommentEntry(item.content);
        
        ofs << "//===================================" << endl;
        ofs << "// " << item.content << endl;
        ofs << "//===================================" << endl;
        ofs << endl;
      }
      else if (item.type == Yuna3GenericString::type_marker) {
        dst.addMarkerEntry(item.content);
        
        ofs << "// === MARKER: " << item.content << endl;
        ofs << endl;
      }
    }
  }
  
protected:
  
};

/*void readGenericMainPtrTable(TStream& src, Yuna3GenericStringSet& dst,
                             const TThingyTable& table,
                             int start, int end) {
  int size = (end - start) / 4;
  for (int i = 0; i < size; i++) {
    int target = start + (i * 4);
    src.seek(target);
    int offset = src.readu32le() - mainExeBaseAddr;
//    if (offset >= src.size()) continue;
//    dst.addString(src, table, offset);
    dst.addPointerTableString(src, table, offset, target);
  }
}

void readGenericMainPtrTableRev(TStream& src, Yuna3GenericStringSet& dst,
                             const TThingyTable& table,
                             int start, int end) {
  int size = (end - start) / 4;
  // these tables are in reverse order for whatever compiler-related reason
  for (int i = size - 1; i >= 0; i--) {
    int target = start + (i * 4);
    src.seek(target);
    int offset = src.readu32le() - mainExeBaseAddr;
//    if (offset >= src.size()) continue;
//    dst.addString(src, table, offset);
    dst.addPointerTableString(src, table, offset, target);
  }
} */

void readGenericStringBlock(TStream& src, Yuna3GenericStringSet& dst,
                             const TThingyTable& table,
                             int start, int end) {
  src.seek(start);
  while (src.tell() < end) {
    int offset = src.tell();
    dst.addString(src, table, offset);
    
    // ignore null padding
    // (strings are padded to next word boundary)
    while (!src.eof() && (src.peek() == 0x00)) src.get();
  }
}

void readGenericStringBlockPtrOvr(TStream& src, Yuna3GenericStringSet& dst,
                             const TThingyTable& table,
                             int start, int end) {
  src.seek(start);
  while (src.tell() < end) {
    int offset = src.tell();
//    dst.addString(src, table, offset);
    
    // new string needed
    Yuna3GenericString result = dst.readString(src, table, offset);
    result.pointerRefs.push_back(offset);
    dst.strings.push_back(result);
    
    // ignore null padding
    // (strings are padded to next word boundary)
    while (!src.eof() && (src.peek() == 0x00)) src.get();
  }
}

/*void addGenericStringToSet(TStream& src, Yuna3GenericStringSet& dst,
                      const TThingyTable& table,
                      int offset
//                      std::string idPrefix
                      ) {
//  std::string idString = idPrefix
//    + "-"
//    + TStringConversion::intToString(offset);
  
  Yuna3GenericString result;
  result.offset = offset;
  
  src.seek(offset);
  while (!src.eof()) {
    if (src.peek() == 0x00) {
      src.get();
      result.size = src.tell() - offset;
      dst.strings.push_back(result);
      return;
    }
    
    TThingyTable::MatchResult matchCheck
      = table.matchId(src);
    if (matchCheck.id == -1) break;
    
    result.content += table.getEntry(matchCheck.id);
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "addGenericString()",
                          std::string("bad string at ")
                          + TStringConversion::intToString(offset));
} */

void dumpString(TStream& ifs, Yuna3GenericStringSet& strings,
                std::string idOverride = "") {
  std::ostringstream ofs;
  std::ostringstream preOfs;
  std::ostringstream contentOfs;
  std::ostringstream postOfs;
  int startPos = ifs.tell();
  
//  std::cerr << std::hex << startPos << std::endl;
  
  bool atLineStart = true;
  bool onCommentLine = false;
  bool literalsNotColors = false;
  while (true) {
    char next = ifs.get();
    if (next == 0x00) break;
    
    if (next > 0) {
      if (next == '\x0A') {
        // linebreak
        ofs << "[br]" << endl;
        atLineStart = true;
        onCommentLine = false;
        
        contentOfs << "[br]" << endl;
      }
      else {
        if (!literalsNotColors
            && (isdigit(next)
                || (next == 'A')
                || (next == 'B')
                || (next == 'C')
                || (next == 'D')
                || (next == 'E')
                || (next == 'F'))
            ) {
          if (onCommentLine) {
            ofs << endl;
            onCommentLine = false;
          }
          ofs << "[color" << next << "]" << endl;
          onCommentLine = false;
          atLineStart = true;
          
          if (!contentOfs.str().size()) {
            preOfs << "[color" << next << "]";
          }
          else {
            contentOfs << "[color" << next << "]" << endl;
          }
        }
        else {
//          if (atLineStart) {
//            ofs << "// ";
//            atLineStart = false;
//            onCommentLine = true;
//          }
          if (atLineStart) {
            atLineStart = false;
            onCommentLine = false;
          }
          
          ofs << next;
          contentOfs << next;
          
          literalsNotColors = true;
        }
      }
    }
    else {
      // 2-byte sjis sequence
      
      char nextnext = ifs.get();
      unsigned char uNext = next;
      unsigned char uNextNext = nextnext;
      
      if ((uNext == 0x81) && (uNextNext == 0xA5)) {
        // "more" indicator"
        
        if (!atLineStart) {
          ofs << endl;
        }
        ofs << endl;
        
        ofs << "[more]" << endl;
        atLineStart = true;
        onCommentLine = false;
        
        // FIXME: is this assumption safe?
        postOfs << "[more]";
      }
      else {
        if (atLineStart) {
          ofs << "// ";
          atLineStart = false;
          onCommentLine = true;
        }
        
        ofs << next << nextnext;
        contentOfs << next << nextnext;
      }
    }
  }
  
  if (!atLineStart) ofs << endl;
  ofs << endl;
  
  // literal string flag
  // i'm assuming these bajillion little strings with "f63 = 0"
  // and the like are actually used for setting/evaluating
  // conditions or something, so flag them as such
  bool isLiteral = literalsNotColors;
  
  // add translation string
/*  int srcId = -1;
  if (!isLiteral) {
    YunaTranslationString transStr;
    transStr.id = translationSheet.nextEntryId();
    srcId = transStr.id;
    transStr.sharedContentPre = preOfs.str();
    transStr.sharedContentPost = postOfs.str();
//    transStr.original = contentOfs.str();
    // convert from SJIS to UTF8
//    std::cerr << translationSheet.nextEntryId() << endl;
    {
      std::string origRaw = contentOfs.str();
      TBufStream conv;
      conv.writeString(origRaw);
      conv.seek(0);
//      cerr << conv.size() << endl;
      while (!conv.eof()) {
        if (conv.peek() == '\x0A') {
          transStr.original += conv.get();
        }
        else if (conv.peek() == '[') {
          std::string name;
          while (!conv.eof()) {
            char next = conv.get();
            name += next;
            if (next == ']') break;
          }
          transStr.original += name;
        }
        else {
          TThingyTable::MatchResult result = tableSjisUtf8.matchId(conv);
          transStr.original += tableSjisUtf8.getEntry(result.id);
        }
      }
    }
    translationSheet.addEntry(transStr);
  } */
  
//  std::cerr << contentOfs.str() << std::endl;
  std::string newStr;
  if (!isLiteral) {
    // convert from SJIS to UTF8
    {
      std::string origRaw = contentOfs.str();
      TBufStream conv;
      conv.writeString(origRaw);
      conv.seek(0);

      while (!conv.eof()) {
        if (conv.peek() == '\x0A') {
          newStr += conv.get();
        }
        else if (conv.peek() == '[') {
          std::string name;
          while (!conv.eof()) {
            char next = conv.get();
            name += next;
            if (next == ']') break;
          }
          newStr += name;
        }
        else {
          TThingyTable::MatchResult result = tableSjisUtf8.matchId(conv);
          newStr += tableSjisUtf8.getEntry(result.id);
        }
      }
    }
    
    newStr = preOfs.str() + newStr + postOfs.str();
  }
  else {
    newStr = contentOfs.str();
  }
  
/*  outofs << "#STARTSTRING(" << index
    << ", "
    <<  TStringConversion::intToString(
          (srcOffsetOverride == -1) ? startPos : srcOffsetOverride,
          TStringConversion::baseHex)
    << ", "
    <<  TStringConversion::intToString(ifs.tell() - startPos,
          TStringConversion::baseHex)
    << ", "
    << TStringConversion::intToString(isLiteral)
    << ")" << endl << endl;
  
  if (isLiteral) {
    outofs << ofs.str();
  }
  else {
    outofs << "#IMPORTSTRING(" << srcId << ")" << endl;
  }
  
  outofs << endl << "#ENDSTRING()" << endl << endl; */
  
//  std::cerr << newStr << std::endl;

//  strings.addRawString(newStr, startPos, ifs.tell() - startPos);
  
  Yuna3GenericString result;
  result.type = Yuna3GenericString::type_string;
  result.content = newStr;
  result.offset = startPos;
  result.size = ifs.tell() - startPos;
  result.idOverride = idOverride;
  strings.strings.push_back(result);
}

std::string getRawString(TStream& ifs, int offset) {
  ifs.seek(offset);
  std::string result;
  char next;
  while (!ifs.eof() && ((next = ifs.get()) != 0)) result += next;
  return result;
}

std::string getRawStringWithConversions(TStream& ifs, int offset) {
  ifs.seek(offset);
  std::string result;
  
  char next;
  while (!ifs.eof() && ((next = ifs.get()) != 0)) {
    if (!ifs.eof() && ((unsigned char)next >= 0x80)) {
      // 2b sjis sequence
      result += next;
      result += ifs.get();
    }
    else {
      if (!ifs.eof() && (next == '\\')) {
        char nextnext = ifs.get();
        
        // insert linebreaks after linebreak commands
        if ((nextnext == 'n') || (nextnext == 'N')) {
          result += next;
          result += nextnext;
          result += '\n';
        }
        // some kind of box break
        else if ((nextnext == 'l') || (nextnext == 'L')) {
          result += next;
          result += nextnext;
          result += ifs.get();
          result += ifs.get();
          result += "\n\n";
        }
        else {
          result += next;
          ifs.unget();
        }
      }
      else {
        result += next;
      }
    }
  }
  
  return result;
}

struct BoxStringResult {
  std::string content;
  std::vector<int> pointerRefs;
//  int originalContentOffset;
  int originalContentSize;
  int originalPos;
  std::vector<FreeSpaceSpec> freeSpaces;
};

void dumpBoxString(const BoxStringResult& boxString,
                   Yuna3GenericStringSet& strings,
                   std::string idPrefix = "",
                   int offset = -1,
                   bool doBoxConversion = false) {
  Yuna3GenericString result;
  result.type = Yuna3GenericString::type_string;
//  result.offset = offset;
  // for technical reasons, the offset must be "valid" (not -1).
  // Yuna3ScriptReader interprets an offset of -1 as a placeholder for
  // "no string" and will not output anything if it is used.
  // so even though these fields are not used, they need to be filled.
  // (now done in constructor)
//  result.offset = 0;
//  result.size = 0;
  
  for (std::vector<int>::const_iterator it = boxString.pointerRefs.cbegin();
       it != boxString.pointerRefs.cend();
       ++it) {
    result.pointerRefs.push_back(*it);
  }
  
  for (std::vector<FreeSpaceSpec>::const_iterator it
        = boxString.freeSpaces.cbegin();
       it != boxString.freeSpaces.cend();
       ++it) {
    result.freeSpaces.push_back(*it);
  }
  
  std::string content = boxString.content;
  
  result.content = content;
  result.doBoxAnalysis = doBoxConversion;
  
  result.idOverride = idPrefix + "-"
    + TStringConversion::intToString(offset,
        TStringConversion::baseHex);
  
  strings.strings.push_back(result);
}

void dumpNonScriptBoxString(TStream& ifs, int offset,
                   Yuna3GenericStringSet& strings,
                   std::string idPrefix = "",
                   bool doBoxConversion = true) {
  Yuna3GenericString result;
  result.type = Yuna3GenericString::type_string;
  
  std::string content = getRawString(ifs, offset);
  result.offset = offset;
  int offsetAfterRedirect = offset + 4;
  // NOTE: includes terminator
  int rawSize = content.size() + 1;
  int sizeAfterRedirect = rawSize - 4;
  result.size = rawSize;
  
  if (sizeAfterRedirect > 0) {
    strings.addAddFreeSpace(offsetAfterRedirect, sizeAfterRedirect);
  }
  
  result.content = content;
  result.doBoxAnalysis = doBoxConversion;
  
  result.idOverride = idPrefix + "-"
    + TStringConversion::intToString(offset,
        TStringConversion::baseHex);
  
  strings.strings.push_back(result);
  
  // TEST
//  ifs.seek(offset);
//  for (int i = 0; i < content.size() + 1; i++)
//    ifs.put(' ');
}

void dumpNonScriptBoxStringSet(TStream& ifs, int offset, int lastOffset,
                   Yuna3GenericStringSet& strings,
                   std::string idPrefix = "",
                   bool doBoxConversion = true) {
  ifs.seek(offset);
  while (ifs.tell() <= lastOffset) {
    if (ifs.peek() == 0x00) {
      ifs.get();
      continue;
    }
    
    dumpNonScriptBoxString(ifs, ifs.tell(), strings, idPrefix, doBoxConversion);
  }
}

void dumpNonScriptBoxStringSetRev(TStream& ifs, int offset, int lastOffset,
                   Yuna3GenericStringSet& strings,
                   std::string idPrefix = "",
                   bool doBoxConversion = true) {
  std::vector<int> stringList;
  ifs.seek(offset);
  while (ifs.tell() <= lastOffset) {
    if (ifs.peek() != 0x00)
      stringList.push_back(ifs.tell());
    while (ifs.get() != 0x00);
  }

  for (int i = stringList.size() - 1; i >= 0; i--) {
    dumpNonScriptBoxString(ifs, stringList[i], strings, idPrefix, doBoxConversion);
  }
}

void dumpNonScriptPtrTable(TStream& ifs, int offset, int size, int ptrBase,
                   Yuna3GenericStringSet& strings,
                   std::string idPrefix = "",
                   bool doBoxConversion = true) {
  for (int i = 0; i < size; i++) {
    ifs.seek(offset + (i * 4));
    int strPtr = ifs.readu32le();
    int strOffset = strPtr - ptrBase;
    dumpNonScriptBoxString(ifs, strOffset, strings, idPrefix, doBoxConversion);
  }
}

void dumpSingleBoxString(TStream& ifs, int addr,
    int strOffset, int strPtrOffset,
    std::map<int, BoxStringResult>& boxStrings,
    std::map<int, BoxStringResult>& freeSpaceBoxStrings) {
  std::string str;
  int totalSize = 0;
  
  std::string nextStr = getRawString(ifs,
    addr);
  // +1 to account for terminator
  freeSpaceBoxStrings[strOffset].originalContentSize
    = nextStr.size() + 1;
  totalSize += nextStr.size() + 1;
  str += nextStr;
  
  // pointer = first line pointer
//  int strPtrOffset = op.offset + 4;
  
  BoxStringResult& result = boxStrings[strOffset];
  
  result.content = str;
  result.pointerRefs.push_back(strPtrOffset);
  result.originalPos = strOffset;
  result.originalContentSize = totalSize;
  
//  boxStrings.push_back(result);
}

void dumpSingleBoxStringOp(TStream& ifs, Yuna3ScriptOp& op,
    int targetOpParamIndex,
    int unindexedBlockBaseAddr,
    std::map<int, BoxStringResult>& boxStrings,
    std::map<int, BoxStringResult>& freeSpaceBoxStrings) {
  dumpSingleBoxString(ifs,
    unindexedBlockBaseAddr + op.params[targetOpParamIndex],
    op.params[targetOpParamIndex], op.offset + 4 + (targetOpParamIndex * 2),
    boxStrings, freeSpaceBoxStrings);
}

//void dumpTextBlock(TStream& ifs, std::ostream& ofs, int blockNum) {
void dumpTextBlock(TStream& ifs, Yuna3GenericStringSet& strings,
                   int blockNum) {
//  strings.addCommentWide(
  strings.addComment(
               string("Text block ")
                + TStringConversion::intToString(blockNum)
                + " (sector "
                + TStringConversion::intToString(
                    textBlockBaseSector
                    + ((blockNum * textBlockSize) / sectorSize),
                    TStringConversion::baseHex)
                + ")");
  
  int blockBaseAddr = (textBlockBaseSector * sectorSize)
                        + (blockNum * textBlockSize);
  ifs.seek(blockBaseAddr);
  
  int scriptDataPos = ifs.tell() + 6;
  int scriptDataSize = ifs.readu16le();
  int unindexedStringBlockOffset = scriptDataSize + 6;
  int stringIndexBlockOffset = unindexedStringBlockOffset + ifs.readu16le();
  int indexedStringBlockOffset = stringIndexBlockOffset + ifs.readu16le();
  
  std::cout << "dumping block " << blockNum
//    << " (unindexed = "
//    << TStringConversion::intToString(unindexedStringBlockOffset,
//        TStringConversion::baseHex)
    << std::endl;
  
  //=====
  // script data
  //=====
  
  ifs.seek(scriptDataPos);
  Yuna3Script script;
  script.read(ifs, scriptDataSize);
  
  // analyze:
  // - identify all strings used by op 0x16 = print
  //   - concatenate each box into a single string
  //   - preprocess for nametags
  //   - create map of first string addr -> list of callers
  //     (not sure if recycled strings exist but best to account for it)
  // - identify all strings used by op 0x11 (args[1] = string offset)
  //   - create map of string addr -> list of callers
  // - all other strings are literals such as condition strings
  //   or filenames and do not matter
  
  // - okay, optimized boxes are a problem.
  //   we need to make them unique.
  //   add every box string we read to a list, regardless
  //   of uniqueness of contents, then handle those sequentially.
  //   separately generate a free space list with all substrings.
  
//  std::map<int, BoxStringResult> boxStrings;
  std::map<int, BoxStringResult> freeSpaceBoxStrings;
  std::map<int, BoxStringResult> boxStrings;
  std::map<int, BoxStringResult> menuStrings;
  
  for (int i = 0; i < script.ops.size(); i++) {
    Yuna3ScriptOp op = script.ops[i];
    
    // ?
    if (op.opcode == 0x02) {
      // i don't understand how the fuck this works.
      // it's called with all kinds of bizarre sizes, and only some
      // actually result in printing.
      // it appears that size must be 5 for printing to happen,
      // but i 100% expect it to turn out this is only the most common case
      // and the actual format is something absurdly complicated.
//      if (op.params.size() < 2) continue;
      if (op.params.size() != 5) continue;
      
      // in any case, if size == 5, then params 0 and 1 are whatever,
      // param 2 is the string to print, param 3 is whatever,
      // and param 4 is the name of the sound file to play for the line
      
/*//      std::cout << "op 02, size " << op.params.size() << std::endl;
      for (int i = 0; i < op.params.size(); i++) {
          std::cout << "  "
            << TStringConversion::intToString(op.params[i],
                 TStringConversion::baseHex)
            << std::endl;
      }
      
      for (int i = 0; i < op.params.size() - 1; i += 2) {
//        if (op.params[i] != 0) {
          dumpSingleBoxStringOp(
            ifs, op, i,
            blockBaseAddr + unindexedStringBlockOffset,
            boxStrings, freeSpaceBoxStrings);
//        }
      }*/
      
      // only certain values of param 3 seem to trigger text printing
      if ((op.params[3] == 0)
          || (op.params[3] == 1)) {
        if ((op.params[1] == 0x10)
            && (op.params[2] == 0x12)) {
          // ??? can happen in e.g. block 29
        }
        else if ((op.params[2] == 0x0)
            || (op.params[2] == 0x4)) {
          // ??? occurs in blocks 55 and 57
        }
        else {
          dumpSingleBoxStringOp(
            ifs, op, 2,
            blockBaseAddr + unindexedStringBlockOffset,
            boxStrings, freeSpaceBoxStrings);
        }
      }
      else {
/*        std::cout << "op 02, size " << op.params.size() << std::endl;
        for (int i = 0; i < op.params.size(); i++) {
            std::cout << "  "
              << TStringConversion::intToString(op.params[i],
                   TStringConversion::baseHex)
              << std::endl;
        }*/
      }
      
/*      if (op.params[2] == 0x12) {
        std::cout << "op 02, size " << op.params.size() << std::endl;
        for (int i = 0; i < op.params.size(); i++) {
            std::cout << "  "
              << TStringConversion::intToString(op.params[i],
                   TStringConversion::baseHex)
              << std::endl;
        }
      }*/
    }
    // op 13 = set menu option
    // op 46 = show menu prompt?
    else if ((op.opcode == 0x13)
             || (op.opcode == 0x46)) {
      int targetParam = 1;
      if (op.opcode == 0x46)
        targetParam = 0;
      
      // params[1] == string offset in unindexed block
      int strOffset = op.params[targetParam];
      std::string str = getRawString(ifs,
        blockBaseAddr + unindexedStringBlockOffset + op.params[targetParam]);
      int strPtrOffset = op.offset + 4 + (targetParam * 2);
      freeSpaceBoxStrings[strOffset].originalContentSize
        = str.size() + 1;
      int totalSize = str.size() + 1;
      
      BoxStringResult& result = menuStrings[strOffset];
      
      result.content = str;
      result.pointerRefs.push_back(strPtrOffset);
      result.originalContentSize = totalSize;
      result.originalPos = strOffset;
      
/*      if (blockNum == 25) {
        std::cout << "op 13, size " << op.params.size() << std::endl;
        for (int i = 0; i < op.params.size(); i++) {
            std::cout << "  "
              << TStringConversion::intToString(op.params[i],
                   TStringConversion::baseHex)
              << std::endl;
        }
      }*/
    }
    // op 17 = print box with default portrait?
    else if (op.opcode == 0x17) {
/*      // each param points to the string for the corresponding line
      // of the text box
      
      if (op.params.size() <= 0) continue;
      
      int strOffset = op.params[0];
      
      std::string str;
      int totalSize = 0;
      for (int j = 0; j < op.params.size(); j++) {
        std::string nextStr = getRawString(ifs,
          blockBaseAddr + unindexedStringBlockOffset + op.params[j]);
        // +1 to account for terminator
        freeSpaceBoxStrings[op.params[j]].originalContentSize
          = nextStr.size() + 1;
        totalSize += nextStr.size() + 1;
        str += nextStr;
        
        // add linebreak if not last line
        if (j != op.params.size() - 1) {
          str += '\n';
        }
      }
      
      // pointer = first line pointer
      int strPtrOffset = op.offset + 4;
      
      BoxStringResult result;
      result.content = str;
      result.pointerRefs.push_back(strPtrOffset);
      // dummy
      result.originalContentSize = -1;*/
      
      if (op.params.size() < 1) continue;
      
//      int strOffset = op.params[0];
      
//      dumpSingleBoxString(
//        ifs, blockBaseAddr + unindexedStringBlockOffset + op.params[0],
//        strOffset, op.offset + 4 + (0 * 2),
//        boxStrings, freeSpaceBoxStrings);

      if (op.params.size() <= 1) {
        dumpSingleBoxStringOp(
          ifs, op, 0,
          blockBaseAddr + unindexedStringBlockOffset,
          boxStrings, freeSpaceBoxStrings);
      }
      else {
        if ((op.params[1] == 0)
            || (op.params[1] == 1)
            || (op.params[1] == 0xD2)) {
          // NOTE: the only instance of a param other than 0, 1, or 3
          // is in block 30, which has an op with (0xCCA, 0xD2).
          // 0xCCA is a valid string, so i guess we have to accept this.
          dumpSingleBoxStringOp(
            ifs, op, 0,
            blockBaseAddr + unindexedStringBlockOffset,
            boxStrings, freeSpaceBoxStrings);
        }
        else {
/*          if (op.params[1] == 3) {
            if (op.params[0] != 2) {
              std::cout << "op 17, size " << op.params.size() << std::endl;
              for (int i = 0; i < op.params.size(); i++) {
                  std::cout << "  "
                    << TStringConversion::intToString(op.params[i],
                         TStringConversion::baseHex)
                    << std::endl;
              }
            }
          }*/
        }
      }
    }
    // op 94 = ? for yuna's interjections in solo boss battle
    else if (op.opcode == 0x94) {
      dumpSingleBoxStringOp(
        ifs, op, 1,
        blockBaseAddr + unindexedStringBlockOffset,
        boxStrings, freeSpaceBoxStrings);
      dumpSingleBoxStringOp(
        ifs, op, 4,
        blockBaseAddr + unindexedStringBlockOffset,
        boxStrings, freeSpaceBoxStrings);
    }
    // op 8B = ??? "switch"??
    // TODO
    // found only in block 81, and appears to take a printable string
    // as its second parameter.
    // whether it's actually used, or needs to be altered, is another matter.
    else if (op.opcode == 0x8B) {
/*      std::cout << "op 8B, size " << op.params.size() << std::endl;
      for (int i = 0; i < op.params.size(); i++) {
          std::cout << "  "
            << TStringConversion::intToString(op.params[i],
                 TStringConversion::baseHex)
            << std::endl;
      }*/
      dumpSingleBoxStringOp(
        ifs, op, 2,
        blockBaseAddr + unindexedStringBlockOffset,
        boxStrings, freeSpaceBoxStrings);
    }
    // op B7 = set death message?
    else if (op.opcode == 0xB7) {
      // param 0 = character index?
      // param 1 = ?
      // param 2 = portrait name
      // param 3 = message
      // param 4 = voice file name
//      dumpSingleBoxStringOp(
//        ifs, op, 2,
//        blockBaseAddr + unindexedStringBlockOffset,
//        boxStrings, freeSpaceBoxStrings);
      dumpSingleBoxStringOp(
        ifs, op, 3,
        blockBaseAddr + unindexedStringBlockOffset,
        boxStrings, freeSpaceBoxStrings);
//      dumpSingleBoxStringOp(
//        ifs, op, 4,
//        blockBaseAddr + unindexedStringBlockOffset,
//        boxStrings, freeSpaceBoxStrings);
    }
    // op BF = ? e.g. "can't teleport here" message (e.g. block 33)
    else if (op.opcode == 0xBF) {
      // TODO
      
      if (op.params[1] != 0xFFFF) {
        dumpSingleBoxStringOp(
          ifs, op, 1,
          blockBaseAddr + unindexedStringBlockOffset,
          boxStrings, freeSpaceBoxStrings);
//        std::cerr << "1: " << op.params[1] << std::endl;
      }
      
      if (op.params[3] != 0xFFFF) {
        dumpSingleBoxStringOp(
          ifs, op, 3,
          blockBaseAddr + unindexedStringBlockOffset,
          boxStrings, freeSpaceBoxStrings);
//        std::cerr << "3: " << op.params[3] << std::endl;
      }
      
      if (op.params[5] != 0xFFFF) {
        dumpSingleBoxStringOp(
          ifs, op, 5,
          blockBaseAddr + unindexedStringBlockOffset,
          boxStrings, freeSpaceBoxStrings);
//        std::cerr << "5: " << op.params[5] << std::endl;
      }
    }
  }
  
  //=====
  // unindexed strings
  //=====
  
  int unindexedStringBlockSize
    = stringIndexBlockOffset - unindexedStringBlockOffset;
  
//  strings.addCommentNarrow(
  strings.addComment(
                 string("Unindexed strings "));
  
//  ofs << "#STARTREGION(" << (blockNum * 2) << ")" << endl
//    << endl;
  strings.addSetRegion(
    std::string("scriptchunk-")
      + TStringConversion::intToString(blockNum,
          TStringConversion::baseHex)
      + "-unindexed");
  
  // HACK: shove all free space definitions into the
  // first available string because the existing script reader
  // isn't set up to handle things being attached directly
  // to the region itself
/*  BoxStringResult* freeDst = NULL;
  if (!menuStrings.empty()) {
    freeDst = &(menuStrings.begin()->second);
  }
  else if (!boxStrings.empty()) {
    freeDst = &(*(boxStrings.begin()));
  }*/
  
  for (std::map<int, BoxStringResult>::iterator it
        = freeSpaceBoxStrings.begin();
       it != freeSpaceBoxStrings.end();
       ++it) {
//    FreeSpaceSpec spec;
//    spec.pos = it->first;
//    spec.size = it->second.originalContentSize;
//    freeDst->freeSpaces.push_back(spec);
    
    strings.addGenericLine(std::string("#ADDFREESPACE(")
      + TStringConversion::intToString(it->first,
          TStringConversion::baseHex)
      + ", "
      + TStringConversion::intToString(it->second.originalContentSize,
          TStringConversion::baseHex)
      + ")");
  }
  
  // check free space at end of block
  // HACK: we know the indexed block will just be a dummy 2b entry
  int indexedBlockEndAddr = blockBaseAddr + indexedStringBlockOffset + 2;
  ifs.seek(blockBaseAddr + textBlockSize - 1);
  {
    int count = 0;
    while ((ifs.tell() >= indexedBlockEndAddr)
           && (ifs.get() == 0x00)) {
      ++count;
      ifs.seekoff(-2);
    }
    
    int endFreeSpaceSize = count - 32;
    if (endFreeSpaceSize > 0) {
      // note that this is addressed in terms of the unindexed block
      strings.addGenericLine(std::string("#ADDFREESPACE(")
        + TStringConversion::intToString(
            (textBlockSize - endFreeSpaceSize) - unindexedStringBlockOffset,
            TStringConversion::baseHex)
        + ", "
        + TStringConversion::intToString(endFreeSpaceSize,
            TStringConversion::baseHex)
        + ")");
    }
  }
  
  // FIXME
  strings.addGenericLine("#SETSIZE(220, 4)");
  
  std::string idPrefix = std::string("block")
      + TStringConversion::intToString(blockNum);
  
  // dump menu strings
  for (std::map<int, BoxStringResult>::iterator it
        = menuStrings.begin();
       it != menuStrings.end();
       ++it) {
    BoxStringResult& result = it->second;
    
    dumpBoxString(result, strings, idPrefix, it->first);
    
    // TEST
//    ifs.seek(blockBaseAddr + unindexedStringBlockOffset + result.originalPos);
//    for (int i = 0; i < result.originalContentSize; i++) {
//      ifs.put(' ');
//    }
  }
  
  // dump box strings
  {
    int num = 0;
    std::map<int, int> seenMap;
    for (std::map<int, BoxStringResult>::iterator it
          = boxStrings.begin();
         it != boxStrings.end();
         ++it) {
      BoxStringResult& result = it->second;
      
      // things have gotten awkward here due to recycling code from
      // yuna 2, which would use the same lines (as in actual rows)
      // of text in different boxes to save space.
      // this game doesn't do that, so it's okay not to deduplicate
      // for instances of the same text being referenced multiple times.
//      if (seenMap.find(result.originalPos) != seenMap.end()) continue;
//      seenMap[result.originalPos] = result.originalPos;
      
      dumpBoxString(result, strings,
//        idPrefix + "-n", num,
        idPrefix + "-n", result.originalPos,
        true);
      ++num;
    
      // TEST
//      ifs.seek(blockBaseAddr + unindexedStringBlockOffset + result.originalPos);
//      for (int i = 0; i < result.originalContentSize; i++) {
//        ifs.put(' ');
//      }
    }
  }
  
//  ofs << "#ENDUNINDEXEDSTRINGBLOCK()" << endl
//    << endl;
  
//  ofs << "#ENDREGION(" << (blockNum * 2) << ")" << endl
//    << endl;
  
  //=====
  // indexed strings
  // (no longer used in this game, so this will always output
  // an empty set)
  //=====
  
  int indexedStringBlockSize
    = indexedStringBlockOffset - stringIndexBlockOffset;
  int numIndexedStrings = indexedStringBlockSize/2;
  
//  addLabelNarrow(ofs,
  strings.addComment(
                 string("Indexed strings "));
  
//  ofs << "#STARTREGION(" << (blockNum * 2) + 1 << ")" << endl
//    << endl;
  strings.addSetRegion(
    std::string("scriptchunk-")
      + TStringConversion::intToString(blockNum,
          TStringConversion::baseHex)
      + "-indexed");
  
  // FIXME
  strings.addGenericLine("#SETSIZE(220, 4)");
  
  ifs.seek(blockBaseAddr + stringIndexBlockOffset);
//  cerr << ifs.tell() << endl;
  
  std::vector<int> stringIndex;
  for (int i = 0; i < numIndexedStrings; i++) {
    stringIndex.push_back(ifs.readu16le());
  }
  
  for (int i = 0; i < numIndexedStrings; i++) {
    if (stringIndex[i] == 0xFFFF) continue;
    ifs.seek(blockBaseAddr + indexedStringBlockOffset + stringIndex[i]);
//    dumpString(ifs, strings, i,
//               ifs.tell() - (blockBaseAddr + indexedStringBlockOffset));
    dumpString(ifs, strings);
  }
  
//  ofs << "#ENDREGION(" << (blockNum * 2) + 1 << ")" << endl
//    << endl;
}

Yuna3GenericString getGenericString(TStream& ifs, int offset) {
  Yuna3GenericString result;
  result.type = Yuna3GenericString::type_string;
  result.offset = offset;
  result.doBoxAnalysis = true;
  
  std::string content;
  content += getRawString(ifs, offset);
  result.content = content;
  result.size = ifs.tell() - offset;
  
  return result;
}

Yuna3GenericString getGenericConvString(TStream& ifs, int offset) {
  Yuna3GenericString result;
  result.type = Yuna3GenericString::type_string;
  result.offset = offset;
  result.doBoxAnalysis = true;
  
  std::string content;
  content += getRawStringWithConversions(ifs, offset);
  result.content = content;
  result.size = ifs.tell() - offset;
  
  return result;
}

void dumpGenericStringWithConversions(
    TStream& ifs, Yuna3GenericStringSet& strings, int offset) {
  strings.strings.push_back(getGenericConvString(ifs, offset));
}

void dumpGenericString(
    TStream& ifs, Yuna3GenericStringSet& strings, int offset) {
  strings.strings.push_back(getGenericString(ifs, offset));
}

void dumpGenericRawString(
    TStream& ifs, Yuna3GenericStringSet& strings, int offset) {
  Yuna3GenericString result = getGenericString(ifs, offset);
  result.doBoxAnalysis = false;
  strings.strings.push_back(result);
}

void dumpMapScript(
    TStream& ifs, Yuna3GenericStringSet& strings,
    std::string idPrefix, TStream& ofs) {
  int stringNum = 0;
  while (!ifs.eof()) {
    std::string line;
    ifs.getLine(line);
    
    TBufStream lineIfs;
    lineIfs.writeString(line);
    lineIfs.seek(0);
    
    TParse::skipSpace(lineIfs);
    if (lineIfs.remaining() < 3) {
      ofs.writeString(line);
      // original scripts use windows linebreaks
      if (!ifs.eof())
        ofs.writeString("\x0D\x0A");
      continue;
    }
    
    std::string start;
    lineIfs.readFixedLenString(start, 3);
    if (!((start.compare("OBS") == 0)
        || (start.compare("GND") == 0))) {
      ofs.writeString(line);
      // original scripts use windows linebreaks
      if (!ifs.eof())
        ofs.writeString("\x0D\x0A");
      continue;
    }
    
    bool isObs = (start.compare("OBS") == 0);
    
    lineIfs.seek(0);
    while (!lineIfs.eof() && isalnum(lineIfs.peek()))
      ofs.put(lineIfs.get());
    
    TParse::matchChar(lineIfs, '(');
    ofs.put('(');
    
    while (!lineIfs.eof() && isdigit(lineIfs.peek()))
      ofs.put(lineIfs.get());
    
    TParse::matchChar(lineIfs, ',');
    ofs.put(',');
    
    std::string rawContent = TParse::matchString(lineIfs);
//    std::string convContent = toUtf8(rawContent);
    
    Yuna3GenericString result;
    std::string idStr = idPrefix
      + "-"
      + (isObs ? "obj" : "gnd")
      + "-"
      + TStringConversion::intToString(stringNum);
//    result.content = convContent;
    result.content = rawContent;
    result.type = Yuna3GenericString::type_string;
    result.idOverride = idStr;
    result.offset = -1;
    result.size = rawContent.size();
    result.doBoxAnalysis = false;
    strings.strings.push_back(result);
    
    ofs.writeString(std::string("\"@[")
      + idStr
      + "]\"");
    
    ++stringNum;
    
    TParse::matchChar(lineIfs, ')');
    ofs.put(')');
    
    while (!lineIfs.eof())
      ofs.put(lineIfs.get());
    
    // original scripts use windows linebreaks
    if (!ifs.eof())
      ofs.writeString("\x0D\x0A");
  }
}

int main(int argc, char* argv[]) {
  if (argc < 1) {
    cout << "Yuna 3 FE script generator" << endl;
//    cout << "Usage: " << argv[0] << " <outprefix>" << endl;
    cout << "Usage: " << argv[0] << endl;
    
    return 1;
  }
  
//  string outprefixName(argv[1]);

  TFileManip::createDirectory("script/orig");
  
  tableSjisUtf8.readUtf8("table/sjis_utf8_yuna3.tbl");
  
//  TBufStream ifs;
//  ifs.open("yuna3_02.iso");
  
  //========================================================================
  // main
  //========================================================================
  
  {
    TBufStream ifs;
    ifs.open("disc/files/DATA/YUNA3.DAT");
    Yuna3GenericStringSet stringsMain;
    Yuna3GenericStringSet stringsSystem;
    
    stringsMain.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
    stringsSystem.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
    
    //=======================================
    // map scripts
    //=======================================
    
    for (int i = 0; i < numTextBlocks; i++) {
      dumpTextBlock(ifs, stringsMain, i);
    }
    
    // TEST
//    ifs.save("test_YUNA3.DAT_blanked");
    
    //=======================================
    // hardcoded script strings
    //=======================================
    
    //=====
    // mission overlays
    //=====
    
    stringsSystem.addGenericLine("#SETSIZE(220, 4)");
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/BMS021.BIN");
      
      stringsMain.addSetRegion("overlay-bms021");
      stringsMain.addComment("Mission 21 overlay strings");
      dumpNonScriptBoxString(ifs, 0x20, stringsMain, "bms021");
      dumpNonScriptBoxString(ifs, 0x64, stringsMain, "bms021");
    }
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/BMS032.BIN");
      
      stringsMain.addSetRegion("overlay-bms032");
      stringsMain.addComment("Mission 32 overlay strings");
      dumpNonScriptBoxString(ifs, 0xC, stringsMain, "bms032");
      dumpNonScriptBoxString(ifs, 0x30, stringsMain, "bms032");
      dumpNonScriptBoxString(ifs, 0x50, stringsMain, "bms032");
      dumpNonScriptBoxString(ifs, 0x90, stringsMain, "bms032");
    }
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/BMS096.BIN");
      
      stringsMain.addSetRegion("overlay-bms096");
      stringsMain.addComment("Mission 96 overlay strings");
      dumpNonScriptBoxString(ifs, 0x20, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x3C, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0xA4, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x164, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x17C, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x188, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x1A0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x1BC, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x1CC, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x1DC, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x1F0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x204, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x21C, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x238, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x248, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x260, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x448, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x488, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x4C0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x4F8, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x530, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x578, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x5BC, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x5F0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x628, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x65c, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x690, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x6d4, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x714, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x750, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x788, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x7bc, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x7f0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x830, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x864, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x89c, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x8d4, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x908, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x944, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x97c, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x9b4, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x9e0, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0xa14, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0xa58, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x5870, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x5890, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x58b8, stringsMain, "bms096");
      dumpNonScriptBoxString(ifs, 0x58d0, stringsMain, "bms096");
    }
    
    //=====
    // missile minigame
    //=====
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/GAME.BIN");
      
      stringsMain.addSetRegion("minigame");
      stringsMain.addComment("Missile minigame overlay strings");
      dumpNonScriptBoxString(ifs, 0x1c, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x60, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0xb8, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0xe4, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x11c, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x164, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x1a4, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x20c, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x244, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x274, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x2bc, stringsMain, "minigame");
      dumpNonScriptBoxString(ifs, 0x2fc, stringsMain, "minigame");
    }
    
    //=====
    // world map
    //=====
    
    stringsSystem.addGenericLine("#SETSIZE(220, 4)");
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/LONGMAP.BIN");
      
      stringsSystem.addSetRegion("worldmap");
      stringsSystem.addComment("World map overlay strings");
      dumpNonScriptBoxString(ifs, 0x20, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x40, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x7c, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0xd4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x12c, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x164, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x194, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x1d8, stringsSystem, "worldmap");
      // easily-missed "e" for location select text
      dumpNonScriptBoxString(ifs, 0x218, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x230, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x244, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x258, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x264, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x270, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x27c, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x290, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2a4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2b8, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2c8, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2d4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2e0, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x2f4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x300, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x330, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x34c, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x3b4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x3e4, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x408, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x428, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x470, stringsSystem, "worldmap");
      // sjis space used for auto-generated padding in location select
      dumpNonScriptBoxString(ifs, 0x789C, stringsSystem, "worldmap");
      dumpNonScriptBoxString(ifs, 0x7e38, stringsSystem, "worldmap");
    }
    
    //=====
    // title
    //=====
    
    stringsSystem.addGenericLine("#SETSIZE(-1, -1)");
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/TITLE.BIN");
      
      stringsSystem.addSetRegion("title");
      
/*      ifs.seek(0x4);
      while (ifs.tell() <= 0xb58) {
        if (ifs.peek() == 0x00) {
          ifs.get();
          continue;
        }
        
        dumpNonScriptBoxString(ifs, ifs.tell(), stringsSystem, "title", false);
      }*/
      
      stringsSystem.addComment("Title -- technique names?");
      dumpNonScriptBoxStringSetRev(ifs, 0x4, 0xB58, stringsSystem, "title", false);
      
      stringsSystem.addComment("Title -- system messages");
      dumpNonScriptBoxString(ifs, 0xba8, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xbbc, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xbd8, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xbf0, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xc3c, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xc54, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xc70, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xc8c, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xcac, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xcc4, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xd2c, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xd44, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xd50, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xd78, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xd90, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xdac, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xdbc, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xdcc, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xde0, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xdf4, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xe10, stringsSystem, "title", false);
      dumpNonScriptBoxString(ifs, 0xe20, stringsSystem, "title", false);
    }
    
    //=====
    // bonus menu
    //=====
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/OMAKE.BIN");
      
      stringsSystem.addSetRegion("bonus");
      stringsSystem.addComment("Bonus menu strings");
      dumpNonScriptBoxString(ifs, 0xd0, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0xe0, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0xf0, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x100, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x110, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x120, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x134, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x148, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x154, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x160, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x16c, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x180, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x194, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x1a8, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x1b8, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x1c4, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x1d0, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x300, stringsSystem, "bonus");
      dumpNonScriptBoxString(ifs, 0x3a0, stringsSystem, "bonus");
    }
    
    //=====
    // card album
    //=====
    
    {
      TBufStream ifs;
      ifs.open("disc/files/BIN/ALBUM.BIN");
      
      stringsSystem.addSetRegion("album");
      stringsSystem.addComment("Card album: card descriptions");
//      dumpNonScriptPtrTable(ifs, 0x4068, 449, 0x80108000, stringsSystem, "album");
      
      stringsSystem.addGenericLine("#SETSIZE(160, 10)");
      
      // card descriptions are stored in reverse order
      std::vector<int> stringList;
      ifs.seek(4);
      while (ifs.tell() <= 0x1564) {
        if (ifs.peek() != 0x00)
          stringList.push_back(ifs.tell());
        while (ifs.get() != 0x00);
      }
      
      for (int i = stringList.size() - 1; i >= 0; i--) {
        dumpNonScriptBoxString(ifs, stringList[i], stringsSystem, "album", false);
      }
      
      stringsSystem.addGenericLine("#SETSIZE(-1, -1)");
      
      stringsSystem.addComment("Card album: menu text");
      
      ifs.seek(0x15D8);
      while (ifs.tell() <= 0x17C0) {
        if (ifs.peek() == 0x00) {
          ifs.get();
          continue;
        }
        
        dumpNonScriptBoxString(ifs, ifs.tell(), stringsSystem, "album", false);
      }
      
      dumpNonScriptBoxString(ifs, 0x17f8, stringsSystem, "album", false);
      dumpNonScriptBoxString(ifs, 0x1818, stringsSystem, "album", false);
      dumpNonScriptBoxString(ifs, 0x19d8, stringsSystem, "album", false);
      dumpNonScriptBoxString(ifs, 0x19f4, stringsSystem, "album", false);
    }
    
    //=======================================
    // main exe text
    //=======================================
    
    {
      TBufStream ifs;
      ifs.open("base/SLPS_014.51_noheader");
      
      stringsSystem.addSetRegion("main");
      
      stringsSystem.addComment("Status-related messages");
//      dumpNonScriptBoxStringSet(ifs, 0xc8, 0x194, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSet(ifs, 0x1AC, 0x1204, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSetRev(ifs, 0x1AC, 0x1204, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSetRev(ifs, 0xc8, 0x194, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0x1138, 0x1204, stringsSystem, "main", true);
      stringsSystem.addComment("Special attack names");
      dumpNonScriptBoxStringSetRev(ifs, 0x718, 0x1124, stringsSystem, "main", true);
      stringsSystem.addComment("Item names");
      dumpNonScriptBoxStringSetRev(ifs, 0x5B0, 0x708, stringsSystem, "main", true);
      stringsSystem.addComment("Unit names");
      dumpNonScriptBoxStringSetRev(ifs, 0x1AC, 0x5A4, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xc8, 0x194, stringsSystem, "main", true);
    
      stringsSystem.addGenericLine("#SETSIZE(220, 4)");
      
      stringsSystem.addComment("Mirage Cannon unlock dialogue");
//      dumpNonScriptBoxStringSet(ifs, 0x1a50, 0x1c54, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSet(ifs, 0x2170, 0x2170, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSet(ifs, 0x241c, 0x24e8, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1a50, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1a94, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1ac4, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1b14, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1b6c, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1ba0, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1c04, stringsSystem, "main", true);
      dumpNonScriptBoxString(ifs, 0x1c54, stringsSystem, "main", true);
      stringsSystem.addComment("? dialogue");
      dumpNonScriptBoxString(ifs, 0x2170, stringsSystem, "main", true);
    
      stringsSystem.addGenericLine("#SETSIZE(-1, -1)");
      
      stringsSystem.addComment("Practice battle text");
      dumpNonScriptBoxStringSet(ifs, 0x241C, 0x24E8, stringsSystem, "main", true);
      
      stringsSystem.addComment("Save prompt");
      dumpNonScriptBoxString(ifs, 0x2850, stringsSystem, "main", true);
      
//      dumpNonScriptBoxStringSet(ifs, 0x3bf4, 0x4178, stringsSystem, "main", true);
      stringsSystem.addComment("Location names");
      dumpNonScriptBoxStringSetRev(ifs, 0x3bf4, 0x3E28, stringsSystem, "main", true);
      stringsSystem.addComment("Save strings");
      dumpNonScriptBoxStringSet(ifs, 0x3E38, 0x4178, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0x41b0, 0x41e0, stringsSystem, "main", true);
      stringsSystem.addComment("Prompt messages");
      dumpNonScriptBoxStringSet(ifs, 0x4708, 0x47a0, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x4c9c, 0x4d3c, stringsSystem, "main", true);
      stringsSystem.addComment("Misc");
      dumpNonScriptBoxStringSet(ifs, 0x5188, 0x5188, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x54d0, 0x54f4, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x5630, 0x5630, stringsSystem, "main", true);
//      dumpNonScriptBoxString(ifs, 0x56b0, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x56b0, 0x56b0, stringsSystem, "main", true);
      stringsSystem.addComment("More special attack dialogue");
      dumpNonScriptBoxStringSet(ifs, 0x5c90, 0x5d5c, stringsSystem, "main", true);
      // TODO: these strings are no longer used in the translation due to being
      // replaced with a more orthogonal system.
      // if we want to be very zealous about conserving space, we can remove these
      // from the script entirely and just turn the whole string block into
      // free space. hopefully not necessary, though.
      stringsSystem.addComment("Attack descriptions");
      dumpNonScriptBoxStringSetRev(ifs, 0x5e28, 0x6aA0, stringsSystem, "main", true);
      stringsSystem.addComment("Labels");
      dumpNonScriptBoxStringSet(ifs, 0x6AB4, 0x6acc, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x6af0, 0x6afc, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0x6c64, 0x6C70, stringsSystem, "main", true);
      stringsSystem.addComment("Item descriptions");
      dumpNonScriptBoxStringSetRev(ifs, 0x6C7C, 0x6EE8, stringsSystem, "main", true);
      stringsSystem.addComment("Action-related error messages");
      dumpNonScriptBoxStringSetRev(ifs, 0x6EF8, 0x6F64, stringsSystem, "main", true);
      stringsSystem.addComment("Card messages");
      dumpNonScriptBoxStringSetRev(ifs, 0x6fd0, 0x6FF0, stringsSystem, "main", true);
      stringsSystem.addComment("Misc messages");
      dumpNonScriptBoxStringSet(ifs, 0x701C, 0x70a8, stringsSystem, "main", true);
      
      stringsSystem.addComment("? labels");
//      dumpNonScriptBoxStringSet(ifs, 0xa2f60, 0xa2f71, stringsSystem, "main", true);
      // FIXME: is this valid?
//      dumpNonScriptBoxString(ifs, 0xa3028, stringsSystem, "main", false);
      dumpNonScriptBoxStringSet(ifs, 0xa3588, 0xa3594, stringsSystem, "main", true);
      
      stringsSystem.addComment("Enemy attack names?");
      dumpNonScriptBoxStringSetRev(ifs, 0xbc4e8, 0xbc634, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc4dc, 0xbc4dc, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc304, 0xbc304, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc24C, 0xbc24C, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc234, 0xbc23C, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc21c, 0xbc21c, stringsSystem, "main", true);
//      dumpNonScriptBoxStringSetRev(ifs, 0xbc21c, 0xbc23c, stringsSystem, "main", true);
      dumpNonScriptBoxStringSetRev(ifs, 0xbc1f4, 0xbc20c, stringsSystem, "main", true);
      stringsSystem.addComment("?");
      dumpNonScriptBoxStringSet(ifs, 0xbc7a0, 0xbc7a8, stringsSystem, "main", true);
      
      dumpNonScriptBoxStringSet(ifs, 0xbc8c0, 0xbc8c8, stringsSystem, "main", true);
      
      dumpNonScriptBoxStringSet(ifs, 0xbcaec, 0xbcafc, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0xbcb1c, 0xbcb24, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0xbcc24, 0xbcc24, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0xbcc3c, 0xbcc3c, stringsSystem, "main", true);
      dumpNonScriptBoxStringSet(ifs, 0xbcd08, 0xbcd10, stringsSystem, "main", true);
      
      stringsSystem.addComment("Status menu");
      dumpNonScriptBoxStringSet(ifs, 0xbd018, 0xbd09c, stringsSystem, "main", true);
      // FIXME: valid?
      dumpNonScriptBoxStringSet(ifs, 0xbd0a4, 0xbd0a8, stringsSystem, "main", true);
      
      // TEST
//      ifs.save("test_SLPS_014.51_blanked");
    }
  
  //========================================================================
  // new strings
  //========================================================================
    
    Yuna3GenericStringSet stringsNew;
    stringsNew.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
    
    {
      stringsNew.addSetRegion("new-status-labels");
      stringsNew.addComment("Additional status screen labels");
      stringsNew.addPlaceholderStrings("status-label", 2);
    }
    
    {
      TBufStream ifs;
      ifs.open("base/SLPS_014.51_noheader");
      
      // HACK: this mess is how i'm dealing with the game's own
      // mess of technique descriptions, which are manually split up
      // into two hardcoded lines apiece for display on the status
      // menu and concatenated into a single line for the action select
      // menu.
      // this is unworkable in translation, so we simplify things
      // so that each two-line description only uses one string, with
      // linebreaks as needed, while a completely separate one-line version
      // is used for the action select menu.
      // it would save some hundreds of bytes to reuse the two-line strings
      // for one-line display by copying to a new buffer and replacing the
      // linebreaks with spaces, unless preceded by a hyphen, or a dash,
      // or an ellipsis, or...
      // and perhaps you see why i don't want to program that if i don't have to.
      //
      // on further reflection: why not just make the status description window
      // as wide as the one-line window, then make the one-line window two lines
      // instead?
      // then we not only have more room, but both windows will accommodate the
      // same strings, so we only need one set.
      std::map<int, int> ptrCheckMap;
      for (int k = 0; k < 1; k++) {
        if (k == 0) {
          stringsNew.addSetRegion("new-techdesc");
          stringsNew.addComment("Standardized technique descriptions");
//          stringsNew.addGenericLine("#SETSIZE(140, 2)");
          stringsNew.addGenericLine("#SETSIZE(256, 2)");
        }
        else {
          stringsNew.addSetRegion("new-techdesc-oneline");
          stringsNew.addComment("Standardized technique descriptions (one-line)");
          stringsNew.addGenericLine("#SETSIZE(280, 1)");
        }
        
        for (int i = 0; i < 29; i++) {
          for (int j = 0; j < 3; j++) {
            int srcOffset = (0x801B3244 - 0x80110000) + (i * 24) + (j * 8);
            ifs.seek(srcOffset);
            int line1Offset = ifs.readu32le() - 0x80110000;
            int line2Offset = ifs.readu32le() - 0x80110000;
            
/*            if (k == 0) {
              if (ptrCheckMap.find(line1Offset) != ptrCheckMap.end()) {
                std::cerr << "repeat line 1 offset: " << std::hex << line1Offset << std::endl;
              }
              else {
                ptrCheckMap[line1Offset] = line1Offset;
              }
              
              if (ptrCheckMap.find(line2Offset) != ptrCheckMap.end()) {
                std::cerr << "repeat line 2 offset: " << std::hex << line2Offset << std::endl;
              }
              else {
                ptrCheckMap[line2Offset] = line2Offset;
              }
            }*/
            
            std::string content;
            content += getRawString(ifs, line1Offset);
            if (k == 0)
              content += "\n";
            content += getRawString(ifs, line2Offset);
            TBufStream temp;
            temp.writeString(content);
  //          temp.seek(0);
  //          dumpGenericStringWithConversions(temp, stringsNew, 0);
  //          dumpGenericRawString(temp, stringsNew, 0);
            Yuna3GenericString result = getGenericString(temp, 0);
            result.doBoxAnalysis = false;
//            result.offset = srcOffset;
            result.pointerRefs.push_back(srcOffset);
            
            std::string id;
            if (k == 0) {
              id = std::string("techdesc-")
                + TStringConversion::intToString(i)
                + "-"
                + TStringConversion::intToString(j);
            }
            else {
              id = std::string("techdesc-oneline-")
                + TStringConversion::intToString(i)
                + "-"
                + TStringConversion::intToString(j);
            }
            
            result.idOverride = id;
            stringsNew.strings.push_back(result);
          }
        }
      }
      
      stringsNew.addSetRegion("unitname-abbrev");
      stringsNew.addComment("Abbreviated unit names");
      for (int i = 0; i < 86; i++) {
        ifs.seek(0x9A828 + (i * 0x60));
        int unitId = ifs.readu32le();
        int namePtr = ifs.readu32le();
        int nameOffset = namePtr - 0x80110000;
        
        dumpNonScriptBoxString(ifs, nameOffset, stringsNew, "unitabbrev", false);
      }
      
      stringsNew.addSetRegion("polysnack");
      stringsNew.addComment("Alternate Polylina Snack messages");
      stringsNew.addPlaceholderStrings("polysnack", 2);
      
      stringsNew.addSetRegion("multiplayer");
      stringsNew.addComment("Alternate multiplayer messages");
      stringsNew.addPlaceholderStrings("multiplayer", 4);
      
      stringsNew.addGenericLine("#SETSIZE(-1, -1)");
    }
  
  //========================================================================
  // map scripts
  //========================================================================
    
    Yuna3GenericStringSet mapScriptStrings;
    
    {
      mapScriptStrings.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
      
      //=======================================
      // map scripts
      //=======================================
      
      TFileManip::FileInfoCollection arcDirListing;
      TFileManip::getDirectoryListing("rsrc_raw/arc/MSDATA", arcDirListing);
      std::sort(arcDirListing.begin(), arcDirListing.end(),
        TFileInfo::sortByIso9660Name);
      for (auto& entry: arcDirListing) {
        if (!entry.isDirectory()) continue;
        
        TFileManip::FileInfoCollection msListing;
        TFileManip::getDirectoryListing(
          (entry.path()).c_str(), msListing);
        
        std::string msName = entry.name();
//        std::cout << msName << std::endl;
        
        for (auto& entry: msListing) {
          std::string name = entry.name();
          if (name.compare("index.txt") == 0) continue;
          
          std::string nameUpper = name;
          for (auto& c: nameUpper) c = toupper(c);
          int dotPos = -1;
          for (int i = 0; i < nameUpper.size(); i++) {
            if (nameUpper[i] == '.') {
              dotPos = i;
              break;
            }
          }
          
          if (dotPos == -1) continue;
          
          std::string extension = nameUpper.substr(dotPos + 1, std::string::npos);
          // due to 8-name limit on filenames in archive files,
          // the target file may have either the extension ".TXT" or ".TX"
          if ((extension.compare("TXT") == 0)
              || (extension.compare("TX") == 0)) {
            mapScriptStrings.addSetRegion(msName + "/" + name);
            
            TBufStream ifs;
            ifs.open(entry.path().c_str());
            TBufStream ofs;
            
            mapScriptStrings.addComment(std::string("Map ")
              + msName);
            dumpMapScript(ifs, mapScriptStrings, msName, ofs);
            
            std::string outScriptName = std::string("script/orig/MSDATA/")
              + msName + "/"
              + name;
            TFileManip::createDirectoryForFile(outScriptName.c_str());
            ofs.save(outScriptName.c_str());
          } 
        }
      }
      
      // TEST
/*      TBufStream ifs;
      TBufStream ofs;
      ifs.open("rsrc_raw/arc/MSDATA/MS03/MS03.TXT");
      dumpMapScript(ifs, mapScriptStrings, "test", ofs);
      ofs.save("test_mapscript.txt");*/
    }
  
  //========================================================================
  // video subtitle placeholders
  //========================================================================
    
    Yuna3GenericStringSet stringsVideo;
    stringsVideo.addGenericLine("#SETSIZE(288, 2)");
    stringsVideo.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
    stringsVideo.addGenericLine("#SETDEFAULTKERNINGOFFSET(-2)");
    
    {
      TFileManip::FileInfoCollection videoDirListing;
      TFileManip::getDirectoryListing("disc/rawfiles/MOVIE", videoDirListing);
      std::sort(videoDirListing.begin(), videoDirListing.end(),
        TFileInfo::sortByIso9660Name);
      for (auto& entry: videoDirListing) {
//        if (entry.isDirectory()) continue;
        
        std::string filename = entry.name();
        std::string labelName = filename.substr(0, filename.size() - 4);
        
        stringsVideo.addSetRegion(labelName);
        stringsVideo.addComment(filename);
        stringsVideo.addPossiblePlaceholderStrings(
//          std::string("video_") + 
          labelName,
          100);
      }
    }
  
  //========================================================================
  // xa subtitle placeholders
  //========================================================================
    
    Yuna3GenericStringSet stringsXa;
    stringsXa.addGenericLine("#SETSIZE(288, 2)");
    stringsXa.addGenericLine("#SETFAILONBOXOVERFLOW(1)");
    stringsXa.addGenericLine("#SETDEFAULTKERNINGOFFSET(-2)");
    stringsXa.addSetRegion("xa");
    
    {
      TBufStream ifs;
      ifs.open("disc/files/DATA/ACMFILE.DAT");
      
      int numFiles = ifs.size() / 16;
      
      std::vector<std::string> xaNames;
      for (int i = 0; i < numFiles; i++) {
        int basePos = ifs.tell();
        std::string filename;
        ifs.readCstrString(filename, 16);
        xaNames.push_back(filename);
        ifs.seek(basePos + 16);
      }
      
      std::sort(xaNames.begin(), xaNames.end());
      
      for (auto& name: xaNames) {
        // HACK: if predumped wave files exist in predetermined location,
        // analyze them to determine start/end times of content
        std::string wavFileName
          = std::string("xa/") + name.substr(0, name.size() - 4) + ".wav";
        double startSec = -1;
        double endSec = -1;
        if (TFileManip::fileExists(wavFileName)) {
          std::cerr << "analyzing: " << wavFileName << std::endl;
          TBufStream ifs;
          ifs.open(wavFileName.c_str());
          
          ifs.seek(0x2C);
          int startSamp = 0;
          while (!ifs.eof()) {
            int nextSamp = ifs.reads16le();
//            if (nextSamp != 0) break;
            if (std::abs(nextSamp) > wavAnalysisNoiseStartThreshold) break;
            ++startSamp;
          }
          
          int endSamp = (ifs.size() - 0x2C) / 2;
          ifs.seek(ifs.size() - 2);
          while (ifs.tell() >= 0x2C) {
            int nextSamp = ifs.reads16le();
            ifs.seekoff(-4);
//            if (nextSamp != 0) break;
            if (std::abs(nextSamp) > wavAnalysisNoiseStopThreshold) break;
            --endSamp;
          }
          
          startSec = (double)startSamp / (double)18900;
          endSec = (double)endSamp / (double)18900;
//          endSec += wavAnalysisEndOffset;
        }
        
        std::string prefix = "";
        std::string suffix = "";
        
        if ((startSec != -1) && (endSec != -1)) {
          prefix += std::string("#P(\"\"startTime\"\", ")
            + "\"\"" + TStringConversion::toString(startSec) + "\"\""
            + ")\n";
          prefix += std::string("#P(\"\"endTime\"\", ")
            + "\"\"" + TStringConversion::toString(endSec) + "\"\""
            + ")";
        }
        
        stringsXa.addPossiblePlaceholderString(
//          std::string("xa_") + 
          name,
          prefix,
          suffix);
      }
    }
    
/*    //=======================================
    // read adv module
    //=======================================
    
    Yuna3GenericStringSet extraStrings;
    
    TBufStream advifs;
    ifs.seek(0x2 * sectorSize);
    advifs.writeFrom(ifs, 0xC * sectorSize);
    
    //=======================================
    // hardcoded strings
    //=======================================
    
//    Yuna3GenericStringSet extraStrings;
//    extraStrings.addComment("Messages for retrying battle?");
//    extraStrings.addSetRegion(-1);
    extraStrings.addGenericLine("#SETSIZE(216, 4)");
    
    extraStrings.addComment("Debug menu + leftovers");
    advifs.seek(0x96FB - 0x4000);
    for (int i = 0; i < 4; i++) {
      dumpGenericRawString(advifs, extraStrings, advifs.tell());
    }
    
    extraStrings.addComment("Save/load prompt");
    advifs.seek(0x456F - 0x4000);
    for (int i = 0; i < 2; i++) {
      dumpGenericRawString(advifs, extraStrings, advifs.tell());
    }
    
    extraStrings.addComment("No files");
    advifs.seek(0x8F9F - 0x4000);
    for (int i = 0; i < 1; i++) {
      dumpGenericRawString(advifs, extraStrings, advifs.tell());
    }
    
    extraStrings.addComment("File names");
    // the file/chapter numbers are written to hardcoded positions
    // within these extraStrings, so they can't be compressed
    extraStrings.addSetNotCompressible(1);
      advifs.seek(0x90AF - 0x4000);
      for (int i = 0; i < 2; i++) {
        dumpGenericRawString(advifs, extraStrings, advifs.tell());
  //      Yuna3GenericString str
  //        = getGenericString(advifs, advifs.tell());
      }
    extraStrings.addSetNotCompressible(0);
    
    extraStrings.addComment("Backup memory errors");
    advifs.seek(0x99DF - 0x4000);
    for (int i = 0; i < 6; i++) {
      dumpGenericRawString(advifs, extraStrings, advifs.tell());
    }

    Yuna3GenericString blankStr;
    blankStr.type = Yuna3GenericString::type_string;
    blankStr.offset = 0;
    blankStr.size = -1;
    
    extraStrings.addComment("Block 1 ask/listen hack");
    for (int i = 0; i < 1; i++) {
      std::string id = std::string("block1-listen-hack-content");
      blankStr.idOverride = id;
      extraStrings.strings.push_back(blankStr);
    }
    
    extraStrings.addComment("Star Bowl subtitles");
//    extraStrings.addSetRegion(-1);
    extraStrings.addGenericLine("#SETSIZE(240, 2)");
    extraStrings.addGenericLine("#SETSCENEMODE(1)");
    extraStrings.addGenericLine("#LOADTABLE(\"table/yuna3_scenes_en.tbl\")");
    extraStrings.addSetNotCompressible(1);

    for (int i = 0; i < 4; i++) {
      std::string id = std::string("starbowl")
        + TStringConversion::intToString(i);
      blankStr.idOverride = id;
      extraStrings.strings.push_back(blankStr);
    }
    
    extraStrings.addComment("Dark queen ending message subtitles");

    for (int i = 0; i < 4; i++) {
      std::string id = std::string("darkqueen")
        + TStringConversion::intToString(i);
      blankStr.idOverride = id;
      extraStrings.strings.push_back(blankStr);
    }
    
    extraStrings.addSetNotCompressible(0);
    extraStrings.addGenericLine("#SETSCENEMODE(0)");*/
    
    //=======================================
    // export
    //=======================================
    
//    scriptSheet.exportCsv("script/orig/script_main.csv");
    
//    for (int i = 0; i < 34; i++) {
    
    Yuna3TranslationSheet scriptSheetMain;
    Yuna3TranslationSheet scriptSheetSystem;
    Yuna3TranslationSheet scriptSheetNew;
    Yuna3TranslationSheet mapScriptSheet;
    Yuna3TranslationSheet scriptSheetVideo;
    Yuna3TranslationSheet scriptSheetXa;
    
    {
      std::ofstream ofs("script/orig/spec_main.txt");
  //    std::ofstream extraOfs("script/orig/spec_main_misc.txt");
      stringsMain.exportToSheet(scriptSheetMain, ofs, "");
  //    extraStrings.exportToSheet(scriptSheet, extraOfs, "adv-");
  //    extraStrings.exportToSheet(scriptSheet, ofs, "adv-");
      scriptSheetMain.exportCsv("script/orig/script_main.csv");
    }
    
    {
      std::ofstream ofs("script/orig/spec_system.txt");
      stringsSystem.exportToSheet(scriptSheetSystem, ofs, "");
      scriptSheetSystem.exportCsv("script/orig/script_system.csv");
    }
    
    {
      std::ofstream ofs("script/orig/spec_mapscript.txt");
      mapScriptStrings.exportToSheet(mapScriptSheet, ofs, "");
      mapScriptSheet.exportCsv("script/orig/script_mapscript.csv");
    }
    
    {
      std::ofstream ofs("script/orig/spec_new.txt");
      stringsNew.exportToSheet(scriptSheetNew, ofs, "");
      scriptSheetNew.exportCsv("script/orig/script_new.csv");
    }
    
    {
      std::ofstream ofs("script/orig/spec_video.txt");
      stringsVideo.exportToSheet(scriptSheetVideo, ofs, "");
      scriptSheetVideo.exportCsv("script/orig/script_video.csv");
    }
    
    {
      std::ofstream ofs("script/orig/spec_xa.txt");
      stringsXa.exportToSheet(scriptSheetXa, ofs, "");
      scriptSheetXa.exportCsv("script/orig/script_xa.csv");
    }
  }
  
  //=======================================
  // add placeholders for new strings
  //=======================================
  
/*  {
    Yuna3GenericStringSet strings;
    
    strings.addGenericLine("#SETFAILONBOXOVERFLOW(1)");

    Yuna3GenericString blankStr;
    blankStr.type = Yuna3GenericString::type_string;
    blankStr.offset = 0;
    blankStr.size = -1;
        
//    strings.addComment("NEW: Ranks for Pom Community");
//    strings.strings.push_back(blankStr);
//    blankStr.offset++;
//    strings.addSetRegion(-1);
    strings.addGenericLine("#SETSIZE(240, 4)");
    strings.addGenericLine("#SETSCENEMODE(1)");
//    strings.addGenericLine("#LOADTABLE(\"table/yuna3_scenes_en.tbl\")");

    for (int i = 0; i < 7; i++) {
      strings.addComment(std::string("Scene ")
        + TStringConversion::intToString(i));
      for (int j = 0; j < 100; j++) {
        std::string id = std::string("scene")
          + TStringConversion::intToString(i)
          + "-"
          + TStringConversion::intToString(j);
        blankStr.idOverride = id;
        strings.strings.push_back(blankStr);
      }
    }
    
    //=======================================
    // export
    //=======================================
    
    Yuna3TranslationSheet scriptSheet;
    
    std::ofstream ofs("script/orig/spec_scene.txt");
    strings.exportToSheet(scriptSheet, ofs, "");
//    extraStrings.exportToSheet(scriptSheet, ofs, "adv-");
    scriptSheet.exportCsv("script/orig/script_scene.csv");
  }*/
  
  return 0;
}
