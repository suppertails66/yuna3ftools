#include "dandy/DandyScriptScanner.h"
#include "dandy/DandyImgExtr.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TPngConversion.h"
#include "util/TSerialize.h"
#include "exception/TGenericException.h"
#include <cctype>

using namespace BlackT;

namespace Psx {


static std::string as4bHex(int num) {
  std::string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, std::string::npos);
  while (str.size() < 4) str = std::string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

static std::string as4bHexPrefix(int num) {
  return "$" + as4bHex(num) + "";
}



DandyScriptString::DandyScriptString()
  : offset(-1),
    content(""),
    triggerOpOffset(-1),
    portraitCode(-1) { }

void DandyScriptString::save(BlackT::TStream& ofs) const {
  ofs.writeu32le(offset);
  TSerialize::writeString(ofs, content);
  ofs.writeu32le(triggerOpOffset);
  ofs.writeu32le(portraitCode);
}

void DandyScriptString::load(BlackT::TStream& ifs) {
  offset = ifs.readu32le();
  TSerialize::readString(ifs, content);
  triggerOpOffset = ifs.readu32le();
  portraitCode = ifs.readu32le();
}

void DandyScriptScanResult::save(BlackT::TStream& ofs) const {
  ofs.writeu32le(strings.size());
  for (unsigned int i = 0; i < strings.size(); i++) {
    strings[i].save(ofs);
  }
  
  ofs.writeu32le(fontMap.size());
  for (LocalToGlobalFontMap::const_iterator it = fontMap.cbegin();
       it != fontMap.cend();
       ++it) {
    ofs.writeu32le(it->first);
    ofs.writeu32le(it->second);
  }
}


void DandyScriptScanResult::load(BlackT::TStream& ifs) {
  strings.resize(ifs.readu32le());
  for (unsigned int i = 0; i < strings.size(); i++) {
    strings[i].load(ifs);
  }
  
  fontMap = LocalToGlobalFontMap();
  int numFontMapEntries = ifs.readu32le();
  for (int i = 0; i < numFontMapEntries; i++) {
    int first = ifs.readu32le();
    int second = ifs.readu32le();
    fontMap[first] = second;
  }
}

/*void DandyScriptScanResult::exportSimpleScript(
          std::ostream& ofs,
          BlackT::TThingyTable& table) const {
  for (ScriptStringCollection::const_iterator it = strings.cbegin();
       it != strings.cend();
       ++it) {
    const DandyScriptString& scriptString = *it;
    
//    TBufStream ifs;
//    ifs.writeString(scriptString.content);
//    ifs.seek(0);
    
    ofs << "#STARTSTRING(" << scriptString.offset
      << ", "
      << scriptString.content.size()
      << ", "
      << scriptString.triggerOpOffset
      << ")"
      << std::endl;
    
    std::string convertedString
      = convertStringFromTable(scriptString.content);
    
    TBufStream ifs;
    ifs.writeString(convertedString);
    ifs.seek(0);
    
    ofs << "// ";
    while (!ifs.eof()) {
      if ((((BlackT::TByte)ifs.peek()) & 0x80) != 0) {
        int codepoint = (ifs.readu16be() & 0x7FFF);
        if (!table.hasEntry(codepoint)) {
          ofs << "<" << as4bHexPrefix(codepoint) << ">";
        }
        else {
          ofs << table.getEntry(codepoint);
        }
      }
      else {
        // ascii
        char next = ifs.get();
        ofs << next;
        
        if (next == 'n') {
          ofs << std::endl << "// ";
        }
        else if (next == 'c') {
          ofs << std::endl << std::endl
//              << "\\c"
//              << std::endl << std::endl
              << "// ";
        }
      }
    }
    
    ofs << std::endl << std::endl;
    
    ofs << "#ENDSTRING()" << std::endl << std::endl;
  }
} */ 

void DandyScriptScanResult::exportSimpleScript(
          std::ostream& ofs,
          BlackT::TThingyTable& table) const {
  int stringNum = 0;
  for (ScriptStringCollection::const_iterator it = strings.cbegin();
       it != strings.cend();
       ++it) {
    const DandyScriptString& scriptString = *it;
    
//    TBufStream ifs;
//    ifs.writeString(scriptString.content);
//    ifs.seek(0);
    
    ofs << "//[TEXT]" << std::endl;
    ofs << "#STARTSTRING("
      << TStringConversion::intToString(scriptString.offset,
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(scriptString.content.size(),
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(scriptString.triggerOpOffset,
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(stringNum,
          TStringConversion::baseHex)
      << ")"
      << std::endl
      << std::endl;
    
    std::string convertedString
      = convertStringFromTable(scriptString.content);
    
    TBufStream ifs;
    ifs.writeString(convertedString);
    ifs.seek(0);
    
//    ofs << "// ";
//    bool atLineStart = true;
    bool inComment = false;
    bool atStart = true;
    bool lastWasLineCode = false;
    while (!ifs.eof()) {
      if ((((BlackT::TByte)ifs.peek()) & 0x80) != 0) {
        if (!inComment) {
          if (!atStart) {
            if (lastWasLineCode)
              ofs << std::endl;
            else
              ofs << std::endl << std::endl;
          }
          ofs << "// ";
        }
        
        int codepoint = (ifs.readu16be() & 0x7FFF);
        if (!table.hasEntry(codepoint)) {
          ofs << "<" << as4bHexPrefix(codepoint) << ">";
        }
        else {
          ofs << table.getEntry(codepoint);
        }
        
        inComment = true;
        atStart = false;
        lastWasLineCode = false;
      }
      else {
        // ascii
        char slash = ifs.get();
        
        if (slash != '\\') {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptScanResult::exportSimpleScript()",
                                  std::string("Unknown ASCII sequence at ")
                                    + TStringConversion::intToString(
                                        ifs.tell()));
        }
        
        char code = ifs.get();
        
        std::string args;
        while (!ifs.eof()
                && ((BlackT::TByte)ifs.peek() < 0x80)
                && (ifs.peek() != '\\')) args += ifs.get();
        
        if (code == 'n') {
          if (!inComment) {
            if (!atStart) {
              if (lastWasLineCode)
                ofs << std::endl;
              else
                ofs << std::endl << std::endl;
            }
            ofs << "// ";
            inComment = true;
          }
          
          ofs << slash << code;
          ofs << std::endl << "// ";
          lastWasLineCode = false;
        }
        else if ((code == '@')
                 || (code == 'p')) {
          if (!inComment) {
            if (!atStart) {
              if (lastWasLineCode)
                ofs << std::endl;
              else
                ofs << std::endl << std::endl;
            }
            ofs << "// ";
            inComment = true;
          }
          
          ofs << slash << code << args;
          lastWasLineCode = false;
        }
        else {
//          if (!inComment) {
//            ofs << std::endl << std::endl << "// ";
//            inComment = true;
//          }
          if (inComment) {
            ofs << std::endl << std::endl;
            inComment = false;
          }
          
          ofs << slash << code << args;
          ofs << std::endl;
          lastWasLineCode = true;
        }
        
        atStart = false;
      }
    }
    
    if (inComment)
      ofs << std::endl;
    ofs << std::endl << std::endl;
    
    ofs << "#ENDSTRING()" << std::endl << std::endl;
    
    ++stringNum;
  }
}

void DandyScriptScanResult::exportHtmlScript(
          std::ostream& ofs,
          BlackT::TThingyTable& table,
          std::string resultNum) const {
  int stringNum = 0;
  for (ScriptStringCollection::const_iterator it = strings.cbegin();
       it != strings.cend();
       ++it) {
    const DandyScriptString& scriptString = *it;
    
//    TBufStream ifs;
//    ifs.writeString(scriptString.content);
//    ifs.seek(0);
    
    ofs << "<div class=\"dandystring\">" << std::endl;
    ofs << "<h3>#STARTSTRING("
      << TStringConversion::intToString(scriptString.offset,
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(scriptString.content.size(),
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(scriptString.triggerOpOffset,
          TStringConversion::baseHex)
      << ", "
      << TStringConversion::intToString(stringNum,
          TStringConversion::baseHex)
      << ")</h3>"
      << "<br>" << std::endl
      << "<br>" << std::endl;
    
    std::string convertedString
      = convertStringFromTable(scriptString.content);
      
    if (scriptString.portraitCode != -1) {
      std::string codeName = resultNum;
      while ((codeName.size() > 0) && (codeName[0] == '0'))
        codeName = codeName.substr(1, std::string::npos); 
      ofs << "<img src=\"portraits_unpacked/"
        << codeName
        << "/1_small/decmp/"
        << scriptString.portraitCode
        << ".png\"><br>" << std::endl;
    }
    
    ofs << "<div class=\"dandystringcontent\">" << std::endl;
    
    TBufStream ifs;
    ifs.writeString(convertedString);
    ifs.seek(0);
    
//    ofs << "// ";
//    bool atLineStart = true;
    bool inComment = false;
    bool atStart = true;
    bool lastWasLineCode = false;
    while (!ifs.eof()) {
      if ((((BlackT::TByte)ifs.peek()) & 0x80) != 0) {
        if (!inComment) {
          if (!atStart) {
            if (lastWasLineCode)
              ofs << "<br>" << std::endl;
            else
              ofs << "<br>" << std::endl << std::endl;
          }
//          ofs << "// ";
        }
        
        int codepoint = (ifs.readu16be() & 0x7FFF);
        if (!table.hasEntry(codepoint)) {
          ofs << "<" << as4bHexPrefix(codepoint) << ">";
        }
        else {
          ofs << table.getEntry(codepoint);
        }
        
        inComment = true;
        atStart = false;
        lastWasLineCode = false;
      }
      else {
        // ascii
        char slash = ifs.get();
        
        if (slash != '\\') {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptScanResult::exportSimpleScript()",
                                  std::string("Unknown ASCII sequence at ")
                                    + TStringConversion::intToString(
                                        ifs.tell()));
        }
        
        char code = ifs.get();
        
        std::string args;
        while (!ifs.eof()
                && ((BlackT::TByte)ifs.peek() < 0x80)
                && (ifs.peek() != '\\')) args += ifs.get();
        
        if (code == 'n') {
          if (!inComment) {
            if (!atStart) {
              if (lastWasLineCode)
                ofs << "<br>" << std::endl;
              else
                ofs << "<br>" << std::endl << std::endl;
            }
//            ofs << "// ";
            inComment = true;
          }
          
          ofs << slash << code;
          ofs << "<br>" << std::endl;
          lastWasLineCode = false;
        }
        else if ((code == '@')
                 || (code == 'p')) {
          if (!inComment) {
            if (!atStart) {
              if (lastWasLineCode)
                ofs << "<br>" << std::endl;
              else
                ofs << "<br>" << std::endl << std::endl;
            }
//            ofs << "// ";
            inComment = true;
          }
          
          ofs << slash << code << args;
          lastWasLineCode = false;
        }
        else {
//          if (!inComment) {
//            ofs << std::endl << std::endl << "// ";
//            inComment = true;
//          }
          if (inComment) {
            ofs << "<br>" << std::endl << std::endl;
            inComment = false;
          }
          
          ofs << slash << code << args;
          ofs << "<br>" << std::endl;
          lastWasLineCode = true;
        }
        
        atStart = false;
      }
    }
    
    if (inComment)
      ofs << "<br>" << std::endl;
    ofs << std::endl << std::endl;
    
    ofs << "</div>" << std::endl;
    
    ofs << "#ENDSTRING()" << "<br>" << std::endl << "<br>" << std::endl;
    ofs << "</div>" << std::endl << std::endl;
    
    ++stringNum;
  }
}

std::string DandyScriptScanResult::convertStringFromTable(
    std::string content) const {
  std::string result;
  
  TBufStream ifs;
  ifs.writeString(content);
  
  TBufStream ofs;
  
  ifs.seek(0);
  while (!ifs.eof()) {
    if ((((BlackT::TByte)ifs.peek()) & 0x80) != 0) {
      // font codepoint
      int localCodepoint = ifs.readu16be();
      int globalCodepoint = fontMap.at(localCodepoint);
      ofs.writeu16be(globalCodepoint | 0x8000);
    }
    else {
      // ascii
      ofs.put(ifs.get());
    }
  }
  
  ofs.seek(0);
  while (!ofs.eof()) result += ofs.get();
  
  return result;
}

int DandyFont::addOrGetChar(const TGraphic& grp) {
  for (unsigned int i = 0; i < fontChars.size(); i++) {
    if (fontChars[i] == grp) return i;
  }
  
  fontChars.push_back(grp);
  return fontChars.size() - 1;
}

void DandyFont::exportFont(std::string outname) const {
  int numRows = fontChars.size() / masterFontCharsPerRow;
  if ((fontChars.size() % masterFontCharsPerRow) != 0) {
    ++numRows;
  }
  
  TGraphic grp(masterFontCharsPerRow * charW,
               numRows * charH);
  grp.clearTransparent();
  
  for (unsigned int i = 0; i < fontChars.size(); i++) {
    int x = (i % masterFontCharsPerRow) * charW;
    int y = (i / masterFontCharsPerRow) * charH;
    grp.copy(fontChars[i],
             TRect(x, y, 0, 0));
  }
  
  TPngConversion::graphicToRGBAPng(outname, grp);
}

BlackT::TGraphic DandyScriptScanner::nullCharGrp = TGraphic(charW, charH);

DandyScriptScanner::DandyScriptScanner() {
//  nullCharGrp.resize(charW, charH);
  nullCharGrp.clear(TColor(0x39, 0x39, 0x39, TColor::fullAlphaOpacity));
}

void DandyScriptScanner::scanScript(BlackT::TStream& ifs, std::string name) {
//  std::cerr << name << std::endl;
  results[name] = DandyScriptScanResult();
  DandyScriptScanResult& result = results[name];
  
  int fontGrpSize = ifs.readu32le();
  int fontBaseAddr = ifs.tell();
  
  TGraphic fontGrp;
  DandyImgExtr::getGraphic(ifs, fontGrp);
  
  // scan the contained characters, add new ones to the master font,
  // and build the local -> master font map
  
  int fontIndexNum = baseFontCodepoint;
  for (int j = 0; j < fontGrp.h(); j += charH) {
    for (int i = 0; i < fontGrp.w(); i += charW) {
      TGraphic charGrp(charW, charH);
      charGrp.copy(fontGrp,
                   BlackT::TRect(0, 0, 0, 0),
                   BlackT::TRect(i, j, charW, charH));
      
      // ignore null characters
      // (this could probably be a break)
      if (charGrp == nullCharGrp) continue;
      
      int masterCharIndex = masterFont.addOrGetChar(charGrp);
      result.fontMap[fontIndexNum++] = masterCharIndex;
    }
  }
  
  // seek to script label block
  ifs.seek(fontBaseAddr + fontGrpSize);
  
  // skip label block
  int numLabels = ifs.readu32le();
  ifs.seekoff(numLabels * 0x14);
  
  // skip unknown value
  ifs.seekoff(4);
  
  // skip unknown word array 1
  int numUnk1Entries = ifs.readu32le();
  ifs.seekoff(numUnk1Entries * 4);
  
  // skip unknown word array 2
  int numUnk2Entries = ifs.readu32le();
  ifs.seekoff(numUnk2Entries * 4);
  
  // we're now at the script block
  
  // do a heuristic scan for strings
  FontCharUseMap fontUseMap;
  int portraitCode = -1;
  while (ifs.remaining() > 2) {
    int basePos = ifs.tell();
    
    int opcode = ifs.readu16le();
    
    int stringCheckPos = -1;
    if (opcode == 0x0001) {
      // standard string print
      
      if (ifs.remaining() < 3) continue;
      
      stringCheckPos = basePos + 2;
    }
    else if (opcode == 0x002C) {
      // ?
      
      if (ifs.remaining() < 3) continue;
      
      stringCheckPos = basePos + 2;
    }
    else if (opcode == 0x005A) {
      // ?
      // takes two 2b params followed by a string
      
      if (ifs.remaining() < 7) continue;
      
      stringCheckPos = basePos + 6;
    }
    else if (opcode == 0x0079) {
      // set portrait num
      
      if (ifs.remaining() < 1) continue;
      
      portraitCode = ifs.readu8();
      
      continue;
    }
    else if (opcode == 0x007A) {
      // clear portrait?
      
      portraitCode = -1;
      continue;
    }
    else {
      // invalid
      ifs.seek(basePos + 1);
      continue;
    }
    
    ifs.seek(stringCheckPos);
    
    DandyScriptString resultString;
    
    FontCharUseMap stringUseMap;
    if (!readScriptString(ifs, resultString, fontIndexNum, stringUseMap)) {
      // invalid
      ifs.seek(basePos + 1);
      continue;
    }
    
//    std::cerr << basePos << " " << ifs.size() << std::endl;
    resultString.triggerOpOffset = basePos;
    resultString.portraitCode = portraitCode;
    result.strings.push_back(resultString);
    
    // add new string's font use map to the overall script map
    for (FontCharUseMap::iterator it = stringUseMap.begin();
         it != stringUseMap.end();
         ++it) {
      fontUseMap[it->first] = it->second;
    }
  }
  
  // ensure every character in the font was used
  for (DandyScriptScanResult::LocalToGlobalFontMap::iterator it
         = result.fontMap.begin();
       it != result.fontMap.end();
       ++it) {
    if (fontUseMap.find(it->first) == fontUseMap.end()) {
      throw TGenericException(T_SRCANDLINE,
                              "DandyScriptScanner::scanScript()",
                              std::string("Unused font character in ")
                                + "'"
                                + name
                                + "' : "
                                + TStringConversion::intToString(it->first,
                                    TStringConversion::baseHex));
    }
  }
}

void DandyScriptScanner::exportMasterFont(std::string outname) const {
  masterFont.exportFont(outname);
}

void DandyScriptScanner::saveScript(BlackT::TStream& ofs) const {
  ofs.writeu32le(results.size());
  for (NameToResultMap::const_iterator it = results.cbegin();
       it != results.cend();
       ++it) {
    TSerialize::writeString(ofs, it->first);
    it->second.save(ofs);
  }
}

void DandyScriptScanner::loadScript(BlackT::TStream& ifs) {
  results = NameToResultMap();
  int numResults = ifs.readu32le();
  for (int i = 0; i < numResults; i++) {
    std::string first;
    TSerialize::readString(ifs, first);
    DandyScriptScanResult second;
    second.load(ifs);
    results[first] = second;
  }
}

void DandyScriptScanner::exportSimpleScript(
        std::ostream& ofs,
        BlackT::TThingyTable& table) const {
  for (NameToResultMap::const_iterator it = results.cbegin();
       it != results.cend();
       ++it) {
    ofs << "//===================================================="
        << std::endl;
    ofs << "// File " << it->first << std::endl;
    ofs << "//===================================================="
        << std::endl;
    ofs << std::endl;
    
    ofs << "#STARTFILE(" << it->first << ")" << std::endl;
    ofs << std::endl;
    
    it->second.exportSimpleScript(ofs, table);
    
    ofs << "#ENDFILE(" << it->first << ")" << std::endl;
    ofs << std::endl;
  }
}

void DandyScriptScanner::exportHtmlScript(
        std::ostream& ofs,
        BlackT::TThingyTable& table) const {
  for (NameToResultMap::const_iterator it = results.cbegin();
       it != results.cend();
       ++it) {
    ofs << "<h2>//====================================================<br>"
        << std::endl;
    ofs << "// File " << it->first << "<br>" << std::endl;
    ofs << "//====================================================<br></h2>"
        << std::endl;
    ofs << "<br>" << std::endl;
    
    
    ofs << "#STARTFILE(" << it->first << ")" << "<br>" << std::endl;
    ofs << "<br>" << std::endl;
    
    ofs << "<div class=\"dandyscript\">" << "<br>" << std::endl;
    it->second.exportHtmlScript(ofs, table, it->first);
    ofs << "</div>" << "<br>" << std::endl;
    
    ofs << "#ENDFILE(" << it->first << ")" << "<br>" << std::endl;
    ofs << "<br>" << std::endl;
  }
}

bool DandyScriptScanner::readScriptString(BlackT::TStream& ifs,
                                          DandyScriptString& dst,
                                          int fontCharLimit,
                                          FontCharUseMap& fontUseMap) {
  // do not allow null strings
  if ((BlackT::TByte)ifs.peek() == 0x00) return false;
  
  dst.offset = ifs.tell();
  
  while (true) {
    if (ifs.eof()) return false;
    
    // terminator
    if ((BlackT::TByte)ifs.peek() == 0x00) {
      break;
    }
    
    // throw out ascii control codes
    if ((BlackT::TByte)ifs.peek() < 0x20) return false;
    
    if ((BlackT::TByte)ifs.peek() >= 0x80) {
      // font char
      if (ifs.remaining() < 2) return false;
      
      int codepoint = ifs.readu16be();
      if (codepoint < baseFontCodepoint) return false;
      if (codepoint >= fontCharLimit) return false;
      
      ifs.seekoff(-2);
      dst.content += ifs.get();
      dst.content += ifs.get();
      
      fontUseMap[codepoint] = true;
    }
    else {
      // ascii
//      dst.content += ifs.get();
      
      // screen for valid sequences
      if (ifs.peek() != '\\') return false;
      dst.content += ifs.get();
      
        if (ifs.remaining() < 1) return false;
      
      char code = ifs.get();
      dst.content += code;
      
      // check for valid stuff
      switch (code) {
      // 2b arg
      case 's':
      case '@':
      case 'v':
      case 'm':
      case 'b': // game recognizes this, though it's not actually used?
        if (ifs.remaining() < 2) return false;
        
        dst.content += ifs.get();
        dst.content += ifs.get();
        break;
      // 2b arg, unless first byte is ascii '0', in which case 4b arg
      case 'p':
      {
        if (ifs.remaining() < 2) return false;
        
        char next = ifs.get();
        dst.content += next;
        
        if (next == '0') {
          if (ifs.remaining() < 3) return false;
          dst.content += ifs.get();
          dst.content += ifs.get();
        }
        
        dst.content += ifs.get();
      }
        break;
      // no args
      case 'w':
      case 'n':
      case 'c':
      case 't':
      case 'q':
        break;
      default:
        return false;
        break;
      }
    }
  }
  
  // skip terminator
  ifs.get();
  
  return true;
}


}
