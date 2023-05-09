#include "yuna3/Yuna3ScriptReader.h"
#include "yuna3/Yuna3LineWrapper.h"
//#include "yuna3/Yuna3MsgConsts.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TPngConversion.h"
#include "util/TFreeSpace.h"
#include "util/TParse.h"
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace BlackT;
using namespace Psx;

const static int sectorSize = 0x800;

const static int textCharsStart = 0x00 + 0x10;
const static int textCharsEnd = textCharsStart + 0x70;
const static int textEncodingMax = 0x100;
const static int maxDictionarySymbols = textEncodingMax - textCharsEnd;

// TODO
const static int fontEmphToggleOp = 0x5F + 8;

const static int op_absoluteStringRedirect = 0x01;
const static int op_tabledStringRedirect = 0x02;
const static int op_subWait = 0x03;
const static int op_subOff = 0x06;
const static int op_subVramOverride = 0x07;

const static int numAdvBlocks = 130;
const static int advBlockSize = 0x8000;

TThingyTable tableStd;
TThingyTable tableScene;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexByteNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  return string("$") + str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}
                      

void binToDcb(TStream& ifs, std::ostream& ofs) {
  int constsPerLine = 16;
  
  while (true) {
    if (ifs.eof()) break;
    
    ofs << "  .db ";
    
    for (int i = 0; i < constsPerLine; i++) {
      if (ifs.eof()) break;
      
      TByte next = ifs.get();
      ofs << as2bHexPrefix(next);
      if (!ifs.eof() && (i != constsPerLine - 1)) ofs << ",";
    }
    
    ofs << std::endl;
  }
}




typedef std::map<std::string, int> UseCountTable;
//typedef std::map<std::string, double> EfficiencyTable;
typedef std::map<double, std::string> EfficiencyTable;

bool isCompressible(std::string& str) {
  for (int i = 0; i < str.size(); i++) {
    if ((unsigned char)str[i] < textCharsStart) return false;
    if ((unsigned char)str[i] >= textCharsEnd) return false;
    if ((unsigned char)str[i] == fontEmphToggleOp) return false;
  }
  
  return true;
}

void addStringToUseCountTable(std::string& input,
                        UseCountTable& useCountTable,
                        int minLength, int maxLength) {
  int total = input.size() - minLength;
  if (total <= 0) return;
  
  for (int i = 0; i < total; ) {
    int basePos = i;
    for (int j = minLength; j < maxLength; j++) {
      int length = j;
      if (basePos + length >= input.size()) break;
      
      std::string str = input.substr(basePos, length);
      
      // HACK: avoid analyzing parameters of control sequences
      // the ops themselves are already ignored in the isCompressible check;
      // we just check when an op enters into the first byte of the string,
      // then advance the check position so the parameter byte will
      // never be considered
/*      if ((str.size() > 0) && ((unsigned char)str[0] < textCharsStart)) {
        unsigned char value = str[0];
        if ((value == 0x02) // "L"
            || (value == 0x05) // "P"
            || (value == 0x06)) { // "W"
          // skip the argument byte
          i += 1;
        }
        break;
      }*/
      if (str.size() > 0) {
        unsigned char value = str[0];
        if ((value != 0) && (value < textCharsStart)) {
          // skip the arguments
//          i += Yuna3MsgConsts::getOpArgsSize(value);
          if (i == op_subVramOverride) i += 4;
          if (i == op_subWait) i += 2;
          ++i;
          break;
        }
      }
      
      if (!isCompressible(str)) break;
      
      ++(useCountTable[str]);
    }
    
    // skip literal arguments to ops
/*    if ((unsigned char)input[i] < textCharsStart) {
      ++i;
      int opSize = numOpParamWords((unsigned char)input[i]);
      i += opSize;
    }
    else {
      ++i;
    } */
    ++i;
  }
}

void addRegionsToUseCountTable(Yuna3ScriptReader::NameToRegionMap& input,
                        UseCountTable& useCountTable,
                        int minLength, int maxLength) {
  for (Yuna3ScriptReader::NameToRegionMap::iterator it = input.begin();
       it != input.end();
       ++it) {
    Yuna3ScriptReader::ResultCollection& results = it->second.strings;
    for (Yuna3ScriptReader::ResultCollection::iterator jt = results.begin();
         jt != results.end();
         ++jt) {
//      std::cerr << jt->srcOffset << std::endl;
      if (jt->isLiteral) continue;
      if (jt->isNotCompressible) continue;
      
      addStringToUseCountTable(jt->str, useCountTable,
                               minLength, maxLength);
    }
  }
}

void buildEfficiencyTable(UseCountTable& useCountTable,
                        EfficiencyTable& efficiencyTable) {
  for (UseCountTable::iterator it = useCountTable.begin();
       it != useCountTable.end();
       ++it) {
    std::string str = it->first;
    // penalize by 1 byte (length of the dictionary code)
    double strLen = str.size() - 1;
    double uses = it->second;
//    efficiencyTable[str] = strLen / uses;
    
    efficiencyTable[strLen / uses] = str;
  }
}

void applyDictionaryEntry(std::string entry,
                          Yuna3ScriptReader::NameToRegionMap& input,
                          std::string replacement) {
  for (Yuna3ScriptReader::NameToRegionMap::iterator it = input.begin();
       it != input.end();
       ++it) {
    Yuna3ScriptReader::ResultCollection& results = it->second.strings;
    int index = -1;
    for (Yuna3ScriptReader::ResultCollection::iterator jt = results.begin();
         jt != results.end();
         ++jt) {
      ++index;
      
      if (jt->isNotCompressible) continue;
      
      std::string str = jt->str;
      if (str.size() < entry.size()) continue;
      
      std::string newStr;
      int i;
      for (i = 0; i < str.size() - entry.size(); ) {
        if ((unsigned char)str[i] < textCharsStart) {
/*          int numParams = numOpParamWords((unsigned char)str[i]);
          
          newStr += str[i];
          for (int j = 0; j < numParams; j++) {
            newStr += str[i + 1 + j];
          }
          
          ++i;
          i += numParams; */
          
/*          newStr += str[i];
          ++i;
          continue;*/
          
/*          if (jt->id.compare("area-0x1F3E-0x7FBC") == 0) {
            std::cerr << "here" << std::endl;
            std::cerr << "op: " << std::hex << (unsigned char)str[i] << std::endl;
            std::cerr << "start:  " << std::hex << i << std::endl;
          }*/
          
          // HACK: assume all ops are linebreak
          int numParams = 1;
          newStr += str[i++];
          for (int j = 0; j < numParams; j++) {
            newStr += str[i + j];
          }
          i += numParams;
          
/*          if (jt->id.compare("area-0x1F3E-0x7FBC") == 0) {
            std::cerr << "size: " << std::dec << numParams << std::endl;
            std::cerr << "finish: " << std::hex << i << std::endl;
            char c;
            std::cin >> c;
          }*/
          
          continue;
        }
        
        if (entry.compare(str.substr(i, entry.size())) == 0) {
          newStr += replacement;
          i += entry.size();
        }
        else {
          newStr += str[i];
          ++i;
        }
      }
      
      while (i < str.size()) newStr += str[i++];
      
      jt->str = newStr;
    }
  }
}

void generateCompressionDictionary(
    Yuna3ScriptReader::NameToRegionMap& results,
    std::string outputDictFileName) {
  TBufStream dictOfs;
  for (int i = 0; i < maxDictionarySymbols; i++) {
//    cerr << i << endl;
    UseCountTable useCountTable;
    addRegionsToUseCountTable(results, useCountTable, 2, 3);
    EfficiencyTable efficiencyTable;
    buildEfficiencyTable(useCountTable, efficiencyTable);
    
//    std::cout << efficiencyTable.begin()->first << std::endl;
    
    // if no compressions are possible, give up
    if (efficiencyTable.empty()) break;  
    
    int symbol = i + textCharsEnd;
    applyDictionaryEntry(efficiencyTable.begin()->second,
                         results,
                         std::string() + (char)symbol);
    
    // debug
/*    TBufStream temp;
    temp.writeString(efficiencyTable.begin()->second);
    temp.seek(0);
//    binToDcb(temp, cout);
    std::cout << "\"";
    while (!temp.eof()) {
      std::cout << table.getEntry(temp.get());
    }
    std::cout << "\"" << std::endl; */
    
    dictOfs.writeString(efficiencyTable.begin()->second);
  }
  
//  dictOfs.save((outPrefix + "dictionary.bin").c_str());
  dictOfs.save(outputDictFileName.c_str());
}

// merge a set of NameToRegionMaps into a single NameToRegionMap
void mergeResultMaps(
    std::vector<Yuna3ScriptReader::NameToRegionMap*>& allSrcPtrs,
    Yuna3ScriptReader::NameToRegionMap& dst) {
  int targetOutputId = 0;
  for (std::vector<Yuna3ScriptReader::NameToRegionMap*>::iterator it
        = allSrcPtrs.begin();
       it != allSrcPtrs.end();
       ++it) {
    Yuna3ScriptReader::NameToRegionMap& src = **it;
    for (Yuna3ScriptReader::NameToRegionMap::iterator jt = src.begin();
         jt != src.end();
         ++jt) {
      dst[TStringConversion::intToString(targetOutputId++)] = jt->second;
    }
  }
}

// undo the effect of mergeResultMaps(), applying any changes made to
// the merged maps back to the separate originals
void unmergeResultMaps(
    Yuna3ScriptReader::NameToRegionMap& src,
    std::vector<Yuna3ScriptReader::NameToRegionMap*>& allSrcPtrs) {
  int targetInputId = 0;
  for (std::vector<Yuna3ScriptReader::NameToRegionMap*>::iterator it
        = allSrcPtrs.begin();
       it != allSrcPtrs.end();
       ++it) {
    Yuna3ScriptReader::NameToRegionMap& dst = **it;
    for (Yuna3ScriptReader::NameToRegionMap::iterator jt = dst.begin();
         jt != dst.end();
         ++jt) {
      jt->second = src[TStringConversion::intToString(targetInputId++)];
    }
  }
}

void exportGenericString(Yuna3ScriptReader::ResultString& str,
                         std::string prefix) {
  if (str.str.size() <= 0) return;
  
  std::string outName = prefix + str.id + ".bin";
  TFileManip::createDirectoryForFile(outName);
  
  TBufStream ofs;
  ofs.writeString(str.str);
  
  if (!str.propertyIsTrue("noTerminator")) {
    ofs.writeu8(0x00);
  }
  
  ofs.save(outName.c_str());
}

void exportGenericRegion(Yuna3ScriptReader::ResultCollection& results,
                         std::string prefix) {
  for (Yuna3ScriptReader::ResultCollection::iterator it = results.begin();
       it != results.end();
       ++it) {
//    if (it->str.size() <= 0) continue;
    Yuna3ScriptReader::ResultString& str = *it;
    exportGenericString(str, prefix);
  }
}

void exportGenericRegionMap(Yuna3ScriptReader::NameToRegionMap& results,
                         std::string prefix) {
  for (auto it: results) {
    exportGenericRegion(it.second.strings, prefix);
  }
}



void readScript(std::string filename, Yuna3ScriptReader::NameToRegionMap& dst,
                BlackT::TThingyTable& table) {
  TBufStream ifs;
  ifs.open(filename.c_str());
  Yuna3ScriptReader(ifs, dst, table)();
}



void freeRegionStringSpaces(Yuna3ScriptReader::ResultRegion& src,
                            TFreeSpace& dst) {
  // free space from all strings, minus the three-byte redirect sequence
  // that will be placed at the start of each one
  for (const auto& str: src.strings) {
    if (str.originalOffset < 0) continue;
    if (str.originalSize <= 3) continue;
    
    int freeStart = str.originalOffset + 3;
    int freeSize = str.originalSize - 3;
    dst.free(freeStart, freeSize);
  }
}

void freeRegionStringSpacesFully(Yuna3ScriptReader::ResultRegion& src,
                            TFreeSpace& dst) {
  // free space from all valid strings
  for (const auto& str: src.strings) {
    if (str.originalOffset < 0) continue;
    
    int freeStart = str.originalOffset;
    int freeSize = str.originalSize;
    dst.free(freeStart, freeSize);
  }
}

void freeRegionSet(Yuna3ScriptReader::NameToRegionMap& src,
                   TFreeSpace& dst) {
  for (auto& item: src) {
    freeRegionStringSpaces(item.second, dst);
  }
}

void replaceAdvBlockString(Yuna3ScriptReader::ResultString& str,
                           int unindexedBlockOffset,
                           TFreeSpace& freeSpace,
                           BlackT::TStream& dst) {
  // need space for string + terminator
  int neededSize = str.str.size() + 1;
  int pos = freeSpace.claim(neededSize);
  if (pos < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "replaceAdvBlockString()",
                            std::string("Not enough free space for adv string: ")
                            + str.id);
  }
  
  dst.seek(unindexedBlockOffset + pos);
  dst.writeString(str.str);
  dst.put(0x00);
  
  for (const auto& refAddr: str.pointerRefs) {
    // account for 6-byte header when computing target address
    dst.seek(refAddr + 6);
    dst.writeu16le(pos);
  }
}

void replaceAdvBlockStrings(Yuna3ScriptReader::ResultRegion& src,
                            BlackT::TStream& dst) {
  dst.seek(0);
  int unindexedBlockOffset = dst.readu16le() + 6;
  TFreeSpace& freeSpace = src.freeSpace;
  
  for (auto& str: src.strings) {
    replaceAdvBlockString(str, unindexedBlockOffset, freeSpace, dst);
  }
}

struct GlobalStringTableEntry {
  enum Type {
    type_null,
    type_string,
    type_pointer
  };
  
  GlobalStringTableEntry()
    : type(type_null) { }
  
  Type type;
  Yuna3ScriptReader::ResultString str;
  int pointerValue;
};

class GlobalStringTable {
public:
  const static int skippedInitialEntries = 257;
  
  int size() const { return entries.size(); }
  
  int addNewString(Yuna3ScriptReader::ResultString& str) {
    skipFillerEntries();
    return addNewStringSub(str);
  }
  
  int addNewQuotedString(Yuna3ScriptReader::ResultString& str) {
    skipFillerAndQuoteEntries();
    return addNewStringSub(str);
  }
  
  int addNewPointer(int pointerValue) {
    skipFillerEntries();
    return addNewPointerSub(pointerValue);
  }
  
  int addNewQuotedPointer(int pointerValue) {
    skipFillerAndQuoteEntries();
    return addNewPointerSub(pointerValue);
  }
  
  void exportTable(TBufStream& dst) {
    TBufStream indexOfs;
    TBufStream dataOfs;
    
    // first 257 entries will always be empty,
    // so don't bother writing them to the output
    int indexSize = (entries.size() - skippedInitialEntries) * 2;
    
    for (int i = skippedInitialEntries; i < entries.size(); i++) {
      GlobalStringTableEntry& entry = entries[i];
      
      int targetDataPos = dataOfs.tell() + indexSize;
      if (entry.type == GlobalStringTableEntry::type_null)
        targetDataPos = 0;
      indexOfs.writeu16le(targetDataPos);
      
      if (entry.type == GlobalStringTableEntry::type_string) {
        dataOfs.writeString(entry.str.str);
        // terminator
        dataOfs.writeu8(0x00);
      }
      else if (entry.type == GlobalStringTableEntry::type_pointer) {
        // high byte will always be 0x80, so only output low 3 bytes
        dataOfs.writeInt(entry.pointerValue & 0x00FFFFFF,
          3, EndiannessTypes::little, SignednessTypes::nosign);
      }
      else if (entry.type == GlobalStringTableEntry::type_null) {
        
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "GlobalStringTable::exportTable()",
                                "impossible type");
      }
    }
    
    // write index and data sequentially
    indexOfs.seek(0);
    dataOfs.seek(0);
    dst.writeFrom(indexOfs, indexOfs.size());
    dst.writeFrom(dataOfs, dataOfs.size());
  }
  
protected:
  std::vector<GlobalStringTableEntry> entries;
  
  void skipFillerEntries() {
    // to try to head off issues with having nulls within strings
    // which may be copied or strcmped by never assigned strings
    // to entries that would have a null in their id --
    // i.e. the first 256 entries and every 256th entry after that
    while ((((entries.size() & 0xFF00) >> 8) == 0)
           || ((entries.size() & 0xFF) == 0)) {
      entries.push_back(GlobalStringTableEntry());
    }
  }
  
  void skipFillerAndQuoteEntries() {
    // like skipFillerEntries, but also skips entries containing
    // an ascii quote (0x22). this is for use in strings that are being
    // inserted into the plaintext mission setup scripts, where
    // strings are bracketed with quotes -- we are replacing the
    // actual strings with table redirects to avoid space issues,
    // and so need to make sure that none of the bytes in the redirect
    // is a 0x22 or else it will be treated as the string terminator.
    while ((((entries.size() & 0xFF00) >> 8) == 0)
           || ((entries.size() & 0xFF) == 0)
           || (((entries.size() & 0xFF00) >> 8) == 0x22)
           || ((entries.size() & 0xFF) == 0x22)) {
      entries.push_back(GlobalStringTableEntry());
    }
  }
  
  int addNewStringSub(Yuna3ScriptReader::ResultString& str) {
    int newId = entries.size();
    
    GlobalStringTableEntry entry;
    entry.type = GlobalStringTableEntry::type_string;
    entry.str = str;
    entries.push_back(entry);
    
//    std::cerr << std::hex << newId << ": " << str.id << std::endl;
    
    return newId;
  }
  
  int addNewPointerSub(int pointerValue) {
    int newId = entries.size();
    
    GlobalStringTableEntry entry;
    entry.type = GlobalStringTableEntry::type_pointer;
    entry.pointerValue = pointerValue;
    entries.push_back(entry);
    
//    std::cerr << std::hex << newId << ": " << std::hex << pointerValue << std::endl;
    
    return newId;
  }
};

/*void replaceGenericBlockString(Yuna3ScriptReader::ResultString& str,
                            BlackT::TStream& dst,
                            TFreeSpace& freeSpace,
                            GlobalStringTable& globalStringTable,
                            int dstLoadAddr) {
  // ensure string is long enough to be replaced at all
  // (need minimum of 3 bytes for 16-bit tabled redirect)
  if (str.originalSize < 3) {
    throw TGenericException(T_SRCANDLINE,
                            "replaceGenericBlockString()",
                            std::string("Unreplaceably short string: ")
                            + str.id);
  }
  
  // need space for string + terminator
  // TODO: unless terminator disabled for string
  int neededSpace = str.str.size() + 1;
  // try to place in dst block
  int dstOffset = freeSpace.claim(neededSpace);
  
  bool doTabledRedirect = false;
  if (dstOffset < 0) doTabledRedirect = true;
  if (str.originalSize < 4) doTabledRedirect = true;
  // TODO: allow strings to be forced into tabled redirect via a string
  // property, for cases where a pointer redirect would cause problems
  // with string copying or size calculations if one of the bytes happens
  // to be null
  
  if (!doTabledRedirect) {
    
  }
  else {
    // place in global table
//    int newStrNum = globalStringTable.size();
//    globalStringTable.entries.push_back(str);
    int newStrNum = globalStringTable.addNewString(str);
    dst.seek(str.originalOffset);
    dst.writeu8(op_absoluteStringRedirect);
    dst.writeu16le(newStrNum);
  }
}*/

void replaceGenericBlockString(Yuna3ScriptReader::ResultString& str,
                            BlackT::TStream& dst,
                            TFreeSpace& freeSpace,
                            GlobalStringTable& globalStringTable,
                            int dstLoadAddr) {
  // ensure string is long enough to be replaced at all
  // (need 4 bytes -- 3 for redirect ops, 1 for terminator
  // to ensure correct behavior for copied strings)
  // FIXME
  // the compiler pads all string start positions to the nearest
  // word boundary, so we should always have 4 bytes to work with
  // and this should be fine...hopefully
/*  if (str.originalSize < 4) {
    throw TGenericException(T_SRCANDLINE,
                            "replaceGenericBlockString()",
                            std::string("Unreplaceably short string: ")
                            + str.id);
  }*/
  
  // need space for string + terminator
  // TODO: unless terminator disabled for string
  int neededSpace = str.str.size() + 1;
  // try to place in dst block
  int dstOffset = freeSpace.claim(neededSpace);
  
  bool doTabledRedirect = false;
  if (dstOffset < 0) doTabledRedirect = true;
  
  if (doTabledRedirect) {
    // place string content directly in global table
//    int newStrNum = globalStringTable.size();
//    globalStringTable.entries.push_back(str);
    int newStrNum = globalStringTable.addNewString(str);
    
    // write redirect sequence
    dst.seek(str.originalOffset);
    dst.writeu8(op_tabledStringRedirect);
    dst.writeu16le(newStrNum);
    dst.writeu8(0x00);
  }
  else {
    // place string content in dst, and add pointer to global table
    dst.seek(dstOffset);
    dst.writeString(str.str);
    // terminator
    dst.writeu8(0x00);
    
    int newPtrNum = globalStringTable.addNewPointer(dstLoadAddr + dstOffset);
    
    // write redirect sequence
    dst.seek(str.originalOffset);
    dst.writeu8(op_absoluteStringRedirect);
    dst.writeu16le(newPtrNum);
    dst.writeu8(0x00);
  }
}

void replaceGenericBlockStrings(Yuna3ScriptReader::ResultRegion& src,
                            BlackT::TStream& dst,
                            GlobalStringTable& globalStringTable,
                            int dstLoadAddr) {
  TFreeSpace& freeSpace = src.freeSpace;
  for (auto& str: src.strings) {
    replaceGenericBlockString(str, dst,
      freeSpace, globalStringTable, dstLoadAddr);
  }
}

// NOTE: target file is overwritten
void replaceGenericFileStrings(Yuna3ScriptReader::ResultRegion& src,
                            std::string targetFileName,
                            GlobalStringTable& globalStringTable,
                            int dstLoadAddr) {
  TBufStream ofs;
  ofs.open(targetFileName.c_str());
  replaceGenericBlockStrings(src, ofs, globalStringTable, dstLoadAddr);
  ofs.save(targetFileName.c_str());
}

void replaceMapScript(Yuna3ScriptReader::ResultRegion& src,
          BlackT::TStream& ifs, BlackT::TStream& ofs,
          GlobalStringTable& globalStringTable) {
  // basically: copy everything literally unless we find an '@'
  // in the input, in which case we need to generate a global string table
  // entry and substitute the symbol with a quote-friendly table reference
  while (!ifs.eof()) {
    if (ifs.peek() != '@') {
      ofs.put(ifs.get());
      continue;
    }
    
    // skip '@'
    ifs.get();
    
    // read symbol name
    TParse::matchChar(ifs, '[');
    std::string symbolName = TParse::getUntilChars(ifs, "]");
//    TParse::matchChar(ifs, ']');
    
    Yuna3ScriptReader::ResultString* strPtr = NULL;
    for (auto& str: src.strings) {
      if (str.id.compare(symbolName) == 0) {
        strPtr = &str;
        break;
      }
    }
    
    if (strPtr == NULL) {
      throw TGenericException(T_SRCANDLINE,
        "replaceMapScript()",
        std::string("Reference to unknown string in map script: ")
        + symbolName);
    }
    
    // this generates a new id entry that specifically does not have
    // the value 0x22 anywhere within it, for compatibility with the
    // quote symbols placed around these strings
    int newStrNum = globalStringTable.addNewQuotedString(*strPtr);
    
    // write redirect sequence
    ofs.writeu8(op_tabledStringRedirect);
    ofs.writeu16le(newStrNum);
    // having a null in the input is safe, hopefully?
    // otherwise i'll have to modify the game code to place a terminator,
    // because it automatically pads strings out with SJIS spaces that we
    // can no longer display
    ofs.writeu8(0x00);
  }
}

void generateAutoInclude(Yuna3ScriptReader::ResultString& str,
                          std::string outStringFilePrefix,
                          std::ostream& stringIncOfs,
                          std::ostream& overwriteOfs,
                          int loadAddr,
                          TFreeSpace* freeSpace = NULL,
                          TBufStream* freeSpaceDst = NULL) {
  // if freeSpace and freeSpaceDst not NULL,
  // try to place string in free space
  if ((freeSpace != NULL) && (freeSpaceDst != NULL)) {
    int neededSize = str.str.size() + 1;
    // TODO: no-terminator check
    
    int dst = freeSpace->claim(neededSize);
    if (dst >= 0) {
      freeSpaceDst->seek(dst);
      freeSpaceDst->writeString(str.str);
      freeSpaceDst->writeu8(0x00);
      
      int dstPtr = dst + loadAddr;
      
      for (const auto& strRef: str.pointerRefs) {
        overwriteOfs << "  .org "
          << TStringConversion::intToString(strRef + loadAddr,
              TStringConversion::baseHex)
          << std::endl;
        
        overwriteOfs << "    .dw "
          << TStringConversion::intToString(dstPtr,
              TStringConversion::baseHex)
          << std::endl;
      }
      
      overwriteOfs << std::endl;
      
      return;
    }
  }
  
  // save string to file
  std::string label = str.id;
  // rrrrrrrrrrrrrrrrrrrrrrrrrrrrrggggggggggggggghhhhhhhhhhhhhhh
  for (auto& c: label) if (c == '-') c = '_';
  std::string outFileName = outStringFilePrefix + label + ".bin";
  TFileManip::createDirectoryForFile(outFileName);
  {
    TBufStream ofs;
    ofs.writeString(str.str);
    ofs.writeu8(0x00);
    // TODO: no-terminator check
    ofs.save(outFileName.c_str());
  }
  
  stringIncOfs << "  " << label << ":" << std::endl;
  stringIncOfs << "    .incbin \"" << outFileName << "\"" << std::endl;
  stringIncOfs << std::endl;
      
  for (const auto& strRef: str.pointerRefs) {
    overwriteOfs << "  .org "
      << TStringConversion::intToString(strRef + loadAddr,
          TStringConversion::baseHex)
      << std::endl;
    
    overwriteOfs << "    .dw "
      << label
      << std::endl;
  }
  
  overwriteOfs << std::endl;
}

// writes all the strings in src to files (prefixed by outFilePrefix),
// and generates two ASM include files: one which sequentially 
// incbins all the output strings with a corresponding label for each,
// and the other overwriting pointer references for each string with
// those labels.
// if freeSpace is not NULL, strings will be inserted into free spaces
// in freeSpaceDst if possible (and not output to disk).
void generateAutoIncludes(Yuna3ScriptReader::ResultRegion& src,
                          std::string outStringFilePrefix,
                          std::string stringIncFileName,
                          std::string overwriteFileName,
                          int loadAddr,
                          TFreeSpace* freeSpace = NULL,
                          TBufStream* freeSpaceDst = NULL) {
  TFileManip::createDirectoryForFile(stringIncFileName);
  TFileManip::createDirectoryForFile(overwriteFileName);
  std::ofstream stringIncOfs(stringIncFileName.c_str());
  std::ofstream overwriteOfs(overwriteFileName.c_str());
  
  for (auto& str: src.strings) {
    generateAutoInclude(str, outStringFilePrefix, stringIncOfs, overwriteOfs,
                        loadAddr, freeSpace, freeSpaceDst);
  }
}


struct StringTableIndexEntry {
  int indexNum;
};

typedef std::map<std::string, StringTableIndexEntry> StringTableIndex;

void exportSubtitleStringTable(Yuna3ScriptReader::ResultRegion& src,
                       std::string outputFilename,
                       StringTableIndex& stringIndex) {
  TBufStream indexOfs;
  TBufStream dataOfs;
  
  int indexNum = 0;
  for (auto& str: src.strings) {
    if (str.str.size() <= 0) continue;
    
    indexOfs.writeu16le(dataOfs.size());
    dataOfs.writeString(str.str);
    // ugh
    dataOfs.writeu8(0x00);
    dataOfs.writeu8(0x00);
    
    StringTableIndexEntry entry;
    entry.indexNum = indexNum;
    stringIndex[str.id] = entry;
    ++indexNum;
  }
  
  indexOfs.seek(0);
  for (int i = 0; i < indexOfs.size() / 2; i++) {
    int num = indexOfs.readu16le();
    indexOfs.seekoff(-2);
    num += indexOfs.size();
    indexOfs.writeu16le(num);
  }
  
  TBufStream ofs;
  indexOfs.seek(0);
  ofs.writeFrom(indexOfs, indexOfs.size());
  dataOfs.seek(0);
  ofs.writeFrom(dataOfs, dataOfs.size());
  
  TFileManip::createDirectoryForFile(outputFilename);
  ofs.save(outputFilename.c_str());
}

/*void exportStandardStringTable(Yuna3ScriptReader::ResultRegion& src,
                       std::string outputFilename) {
  TBufStream indexOfs;
  TBufStream dataOfs;
  
  int indexNum = 0;
  for (auto& str: src.strings) {
    if (str.str.size() == 0) {
      indexOfs.writeu16le(0xFFFF);
      continue;
    }
    
    indexOfs.writeu16le(dataOfs.size());
    dataOfs.writeString(str.str);
    if (!str.propertyIsTrue("noTerminator")) {
      dataOfs.writeu8(0x00);
    }
  }
  
  indexOfs.seek(0);
  for (int i = 0; i < indexOfs.size() / 2; i++) {
    int num = indexOfs.readu16le();
    if (num == 0xFFFF) continue;
    indexOfs.seekoff(-2);
    num += indexOfs.size();
    indexOfs.writeu16le(num);
  }
  
  TBufStream ofs;
  indexOfs.seek(0);
  ofs.writeFrom(indexOfs, indexOfs.size());
  dataOfs.seek(0);
  ofs.writeFrom(dataOfs, dataOfs.size());
  
  TFileManip::createDirectoryForFile(outputFilename);
  ofs.save(outputFilename.c_str());
}*/

// adds the strings in src to globalStringTable,
// and generates and writes to file an indexed table of redirects
// to the new global string table entries.
// null strings are indicated in the table index with 0xFFFF.
void exportStringTableViaGlobalStringTable(Yuna3ScriptReader::ResultRegion& src,
                       std::string outputFilename,
                       GlobalStringTable& globalStringTable) {
  TBufStream indexOfs;
  TBufStream dataOfs;
  
  int indexNum = 0;
  for (auto& str: src.strings) {
    if (str.str.size() == 0) {
      indexOfs.writeu16le(0xFFFF);
      continue;
    }
    
    int newStrNum = globalStringTable.addNewString(str);
    
    indexOfs.writeu16le(dataOfs.size());
    
    dataOfs.writeu8(op_tabledStringRedirect);
    dataOfs.writeu16le(newStrNum);
    dataOfs.writeu8(0x00);
  }
  
  indexOfs.seek(0);
  for (int i = 0; i < indexOfs.size() / 2; i++) {
    int num = indexOfs.readu16le();
    if (num == 0xFFFF) continue;
    indexOfs.seekoff(-2);
    num += indexOfs.size();
    indexOfs.writeu16le(num);
  }
  
  TBufStream ofs;
  indexOfs.seek(0);
  ofs.writeFrom(indexOfs, indexOfs.size());
  dataOfs.seek(0);
  ofs.writeFrom(dataOfs, dataOfs.size());
  
  TFileManip::createDirectoryForFile(outputFilename);
  ofs.save(outputFilename.c_str());
}


int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yuna 3 FE script builder" << endl;
    cout << "Usage: " << argv[0] << " [inprefix] [outprefix]"
      << endl;
    return 0;
  }
  
//  string infile(argv[1]);
  string inPrefix(argv[1]);
  string outPrefix(argv[2]);

  tableStd.readUtf8("table/yuna3_en.tbl");
  tableScene.readUtf8("table/yuna3_scene_en.tbl");
  
  GlobalStringTable globalStringTable;
  
  //=====
  // read script
  //=====
  
  Yuna3ScriptReader::NameToRegionMap mainResults;
  Yuna3ScriptReader::NameToRegionMap mapScriptResults;
  Yuna3ScriptReader::NameToRegionMap newResults;
  Yuna3ScriptReader::NameToRegionMap systemResults;
  Yuna3ScriptReader::NameToRegionMap videoResults;
  Yuna3ScriptReader::NameToRegionMap xaResults;
  
  readScript((inPrefix + "spec_main.txt"), mainResults, tableStd);
  readScript((inPrefix + "spec_mapscript.txt"), mapScriptResults, tableStd);
  readScript((inPrefix + "spec_new.txt"), newResults, tableStd);
  readScript((inPrefix + "spec_system.txt"), systemResults, tableStd);
  readScript((inPrefix + "spec_video.txt"), videoResults, tableStd);
  readScript((inPrefix + "spec_xa.txt"), xaResults, tableStd);
  
/*  Yuna3ScriptReader::NameToRegionMap battleResults;
  Yuna3ScriptReader::NameToRegionMap bossResults;
  Yuna3ScriptReader::NameToRegionMap altBossResults;
  Yuna3ScriptReader::NameToRegionMap creditsResults;
  Yuna3ScriptReader::NameToRegionMap dialogueResults;
  Yuna3ScriptReader::NameToRegionMap enemyResults;
  Yuna3ScriptReader::NameToRegionMap genericTextResults;
  Yuna3ScriptReader::NameToRegionMap infoResults;
  Yuna3ScriptReader::NameToRegionMap introEndingResults;
  Yuna3ScriptReader::NameToRegionMap itemsResults;
  Yuna3ScriptReader::NameToRegionMap menuResults;
  Yuna3ScriptReader::NameToRegionMap miscResults;
  Yuna3ScriptReader::NameToRegionMap saveLoadResults;
  Yuna3ScriptReader::NameToRegionMap shopResults;
  
  readScript((inPrefix + "spec_battle.txt"), battleResults, tableStd);
  readScript((inPrefix + "spec_boss.txt"), bossResults, tableStd);
  readScript((inPrefix + "spec_bossalt.txt"), altBossResults, tableStd);
  readScript((inPrefix + "spec_credits.txt"), creditsResults, tableStd);
  readScript((inPrefix + "spec_dialogue.txt"), dialogueResults, tableStd);
  readScript((inPrefix + "spec_enemy.txt"), enemyResults, tableStd);
  readScript((inPrefix + "spec_generic_text.txt"), genericTextResults, tableStd);
  readScript((inPrefix + "spec_info.txt"), infoResults, tableStd);
  readScript((inPrefix + "spec_intro_ending.txt"), introEndingResults, tableStd);
  readScript((inPrefix + "spec_items.txt"), itemsResults, tableStd);
  readScript((inPrefix + "spec_menu.txt"), menuResults, tableScene);
  readScript((inPrefix + "spec_misc.txt"), miscResults, tableStd);
  readScript((inPrefix + "spec_saveload.txt"), saveLoadResults, tableStd);
  readScript((inPrefix + "spec_shop.txt"), shopResults, tableStd);
  
  //=====
  // read files to be updated
  //=====
  
  TBufStream isoIfs;
  isoIfs.open("yuna3_02_build.iso");
  
  TBufStream battleIfs;
  battleIfs.open("out/base/battle_1D.bin");
  TBufStream battleExtraIfs;
  battleExtraIfs.open("out/base/battle_extra_27.bin");
  TBufStream battleTextIfs;
  battleTextIfs.open("out/base/battle_text_2D.bin");
  TBufStream battleText2Ifs;
  battleText2Ifs.open("out/base/battle_text2_31.bin");
  TBufStream creditsIfs;
  creditsIfs.open("out/base/credits_4F.bin");
//  TBufStream emptyIfs;
//  emptyIfs.open("out/base/empty_2.bin");
  TBufStream genericTextIfs;
  genericTextIfs.open("out/base/generic_text_4B.bin");
  TBufStream infoIfs;
  infoIfs.open("out/base/info_5B.bin");
  TBufStream introIfs;
  introIfs.open("out/base/intro_3B.bin");
  TBufStream kernelIfs;
  kernelIfs.open("out/base/kernel_9.bin");
  TBufStream overwIfs;
  overwIfs.open("out/base/overw_13.bin");
  TBufStream saveLoadIfs;
  saveLoadIfs.open("out/base/saveload_45.bin");
  TBufStream shopIfs;
  shopIfs.open("out/base/shop_19.bin");
  TBufStream specialIfs;
  specialIfs.open("out/base/special_51.bin");
  TBufStream unknown1Ifs;
  unknown1Ifs.open("out/base/unknown1_23.bin");
  TBufStream unknown2Ifs;
  unknown2Ifs.open("out/base/unknown2_35.bin");
  TBufStream unknown3Ifs;
  unknown3Ifs.open("out/base/unknown3_57.bin");*/
  
  //=====
  // compress
  //=====
  
  {
    Yuna3ScriptReader::NameToRegionMap allStrings;
    
    std::vector<Yuna3ScriptReader::NameToRegionMap*> allSrcPtrs;
    allSrcPtrs.push_back(&mainResults);
    allSrcPtrs.push_back(&mapScriptResults);
    allSrcPtrs.push_back(&newResults);
    allSrcPtrs.push_back(&systemResults);
    allSrcPtrs.push_back(&xaResults);
    
    // merge everything into one giant map for compression
    mergeResultMaps(allSrcPtrs, allStrings);
    
    // compress
    generateCompressionDictionary(
      allStrings, outPrefix + "script_dictionary.bin");
    
    // restore results from merge back to individual containers
    unmergeResultMaps(allStrings, allSrcPtrs);
  }
  
  //============================
  // update adv scenes
  //============================
  
  {
    TBufStream ifs;
    ifs.open("out/files/DATA/YUNA3.DAT");
    
    for (int i = 0; i < numAdvBlocks; i++) {
      ifs.seek(i * advBlockSize);
      TBufStream blockIfs;
      blockIfs.writeFrom(ifs, advBlockSize);
      
      std::string srcRegionName = std::string("scriptchunk-")
        + TStringConversion::intToString(i,
            TStringConversion::baseHex)
        + "-unindexed";
      replaceAdvBlockStrings(mainResults.at(srcRegionName), blockIfs);
      
      ifs.seek(i * advBlockSize);
      blockIfs.seek(0);
      ifs.writeFrom(blockIfs, advBlockSize);
    }
    
    ifs.save("out/files/DATA/YUNA3.DAT");
  }
  
  //============================
  // update hardcoded blocks
  //============================
  
  //=====
  // main exe
  //=====
  
  // NOTE: requires special handling to deal with EXE header,
  // which is not included in any offset calculations
  
  TBufStream mainIfs;
  mainIfs.open("out/files/SLPS_014.51");
  
  TBufStream mainIfsNoHeader;
  mainIfs.seek(0x800);
  mainIfsNoHeader.writeFrom(mainIfs, mainIfs.remaining());
  
  replaceGenericBlockStrings(systemResults.at("main"),
                            mainIfsNoHeader,
                            globalStringTable,
                            0x80110000);
  
  //=====
  // overlays
  //=====
  
  replaceGenericFileStrings(mainResults.at("overlay-bms021"),
                            "out/files/BIN/BMS021.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(mainResults.at("overlay-bms032"),
                            "out/files/BIN/BMS032.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(mainResults.at("overlay-bms096"),
                            "out/files/BIN/BMS096.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(mainResults.at("minigame"),
                            "out/files/BIN/GAME.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(systemResults.at("worldmap"),
                            "out/files/BIN/LONGMAP.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(systemResults.at("title"),
                            "out/files/BIN/TITLE.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(systemResults.at("bonus"),
                            "out/files/BIN/OMAKE.BIN",
                            globalStringTable,
                            0x80108000);
  
  replaceGenericFileStrings(systemResults.at("album"),
                            "out/files/BIN/ALBUM.BIN",
                            globalStringTable,
                            0x80108000);
  
  //============================
  // update mission setup scripts
  //============================
  
  for (auto regionPair: mapScriptResults) {
    std::string targetBaseName = regionPair.first;
    Yuna3ScriptReader::ResultRegion& region = regionPair.second;
    
    TBufStream ifs;
    ifs.open(("script/MSDATA/" + targetBaseName).c_str());
    TBufStream ofs;
    
    replaceMapScript(region, ifs, ofs, globalStringTable);
    
    ofs.save(
      (std::string("out/rsrc_raw/arc/MSDATA/") + targetBaseName).c_str());
  }
  
  //============================
  // export generic/hardcoded strings
  //============================
  
  exportGenericRegion(newResults.at("new-status-labels").strings,
                      "out/script/new/");
//  exportGenericRegionMap(newResults,
//                         "out/script/new/");
  
  generateAutoIncludes(newResults.at("new-techdesc"),
                       "out/script/techdesc/",
                       "out/script/asm/techdesc_strings.inc",
                       "out/script/asm/techdesc_overwrite.inc",
                       0x80110000,
                       &(systemResults.at("main").freeSpace),
                       &mainIfsNoHeader);
  
  exportStringTableViaGlobalStringTable(newResults.at("unitname-abbrev"),
                    "out/script/unitabbrev/table.bin",
                    globalStringTable);
  
  exportGenericRegion(newResults.at("polysnack").strings,
                      "out/script/new/");
  
  exportGenericRegion(newResults.at("multiplayer").strings,
                      "out/script/new/");
  
  
  
  exportGenericRegion(systemResults.at("worldmap").strings,
                      "out/script/worldmap/");
  
  //============================
  // export video subtitle strings
  //============================
  
  exportGenericRegionMap(videoResults,
                         "out/script/video/");
  
  //============================
  // export xa subtitles
  //============================
  
//  exportGenericRegionMap(xaResults,
//                         "out/script/xa/");
  
  {
    StringTableIndex xaStringIndex;
    exportSubtitleStringTable(xaResults.at("xa"),
                      "out/script/xa/table.bin",
                      xaStringIndex);
    
    // place results in ACMFILE.DAT
    TBufStream ifs;
    ifs.open("out/files/DATA/ACMFILE.DAT");
    int numFiles = ifs.size() / 16;
    for (int i = 0; i < numFiles; i++) {
      int basePos = ifs.tell();
      std::string filename;
      ifs.readCstrString(filename, 16);
      
      StringTableIndex::iterator it = xaStringIndex.find(filename);
      if (it != xaStringIndex.end()) {
        ifs.seek(basePos + 14);
        // write target index number plus one, because 0 indicates
        // no script
        ifs.writeu16le(it->second.indexNum + 1);
      }
      
      ifs.seek(basePos + 16);
    }
    
    ifs.save("out/files/DATA/ACMFILE.DAT");
  }
  
  //============================
  // save global string table
  //============================
  
  {
    TBufStream ofs;
    globalStringTable.exportTable(ofs);
    ofs.save("out/script/globalstringtable.bin");
  }
  
  //============================
  // save main exe
  //============================
  
  mainIfsNoHeader.seek(0);
  mainIfs.seek(0x800);
  mainIfs.writeFrom(mainIfsNoHeader, mainIfsNoHeader.size());
  mainIfs.save("out/files/SLPS_014.51");
  
/*  //=====
  // update blocks
  //=====
  
//  redirectOneShotScriptBlock(saveLoadResults.begin()->second,
//                             saveLoadIfs,
//                             0x9000);
  redirectSharedScriptBlock(saveLoadResults,
                            saveLoadIfs,
                            0x9000);
  
//  redirectOneShotScriptBlock(introEndingResults.begin()->second,
//                             introIfs,
//                             0x9000);
  redirectSharedScriptBlock(introEndingResults,
                            introIfs,
                            0x9000);
  
//  redirectSharedScriptBlock(genericTextResults,
//                            genericTextIfs,
//                            0xC000);
  {
    TFreeSpace freeSpace = genericTextResults.begin()->second.freeSpace;
//    freeRegionSet(genericTextResults, freeSpace);
    freeRegionStringSpaces(genericTextResults.at("generic_msg"),
                           freeSpace);
    freeRegionStringSpacesFully(genericTextResults.at("generic_techname"),
                           freeSpace);
    
//    for (auto it: freeSpace.freeSpace_) {
//      std::cerr << std::hex << it.first << " " << it.second << std::endl;
//    }
    
    redirectScriptBlock(genericTextResults.at("generic_msg"),
                        genericTextIfs, NULL, 0xC000,
                        &freeSpace, NULL);
    
    redirectTabledScriptBlock(genericTextResults.at("generic_techname"),
                        genericTextIfs, NULL,
                        0xF17, 0x2A, 0xC000,
                        &freeSpace, NULL);
  }
  
  redirectOneShotScriptBlock(battleResults["battle_std"],
                             battleTextIfs,
                             0xC000);
  redirectOneShotScriptBlock(battleResults["battle_boss"],
                             battleText2Ifs,
                             0xC000);
  
  redirectOneShotScriptBlock(miscResults.at("misc_gokumon"),
                             specialIfs,
                             0x9000);
  
  redirectSharedScriptBlock(shopResults,
                            shopIfs,
                            0xC000);
  
  redirectSharedScriptBlock(infoResults,
                            infoIfs,
                            0xC000);
  
  redirectBossMessageSet(bossResults, isoIfs);
  redirectBossMessageSet(altBossResults, isoIfs);
  
  //=====
  // export generic/hardcoded strings
  //=====
  
  exportItemTableInc(itemsResults.begin()->second,
                     "out/script/items/itemlist.inc");
  
  exportGenericRegion(miscResults["misc_yesno_prompt"].strings,
                      "out/script/misc/");
  
  exportGenericRegion(menuResults["menu_kernel"].strings,
                      "out/script/menu/kernel/");
  
  exportGenericRegion(menuResults["menu_overw"].strings,
                      "out/script/menu/overw/");
  
  exportGenericRegion(menuResults["menu_shop"].strings,
                      "out/script/menu/shop/");
  
  exportGenericRegion(menuResults["menu_battle"].strings,
                      "out/script/menu/battle/");
  
  exportGenericRegion(menuResults["menu_saveload"].strings,
                      "out/script/menu/saveload/");
  
  exportGenericRegion(menuResults["menu_info"].strings,
                      "out/script/menu/info/");
  
  exportGenericRegion(menuResults["orochimaru_name"].strings,
                      "out/script/menu/orochimaru_patch/");
  
  //=====
  // regenerate enemy name data
  //=====
  
  regenerateEnemyNameData(enemyResults.begin()->second,
                          battleExtraIfs);
  
  //=====
  // update maps
  //=====
  
  for (auto& nameRegionPair: dialogueResults) {
    Yuna3ScriptReader::ResultRegion& region
      = nameRegionPair.second;
    std::cerr << "redirecting map: " << nameRegionPair.first << std::endl;
    redirectMap(region, isoIfs);
  }
  
  //=====
  // save modified files
  //=====
  
  battleIfs.save("out/base/battle_1D.bin");
  battleExtraIfs.save("out/base/battle_extra_27.bin");
  battleTextIfs.save("out/base/battle_text_2D.bin");
  battleText2Ifs.save("out/base/battle_text2_31.bin");
  creditsIfs.save("out/base/credits_4F.bin");
//  emptyIfs.save("out/base/empty_2.bin");
  genericTextIfs.save("out/base/generic_text_4B.bin");
  infoIfs.save("out/base/info_5B.bin");
  introIfs.save("out/base/intro_3B.bin");
  kernelIfs.save("out/base/kernel_9.bin");
  overwIfs.save("out/base/overw_13.bin");
  saveLoadIfs.save("out/base/saveload_45.bin");
  shopIfs.save("out/base/shop_19.bin");
  specialIfs.save("out/base/special_51.bin");
  unknown1Ifs.save("out/base/unknown1_23.bin");
  unknown2Ifs.save("out/base/unknown2_35.bin");
  unknown3Ifs.save("out/base/unknown3_57.bin");
  
  //=====
  // save modified iso
  //=====
  
  isoIfs.save("yuna3_02_build.iso");*/
  
  return 0;
}
