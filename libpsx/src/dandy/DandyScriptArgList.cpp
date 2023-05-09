#include "dandy/DandyScriptArgList.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <cctype>

using namespace BlackT;

namespace Psx {

  
DandyScriptArgList::DandyScriptArgList() { }

void DandyScriptArgList::read(DandyScriptEnv& env,
            BlackT::TStream& ifs, std::string argString) {
  unsigned int pos = 0;
  while (pos < argString.size()) {
    char c = argString[pos++];
    switch (c) {
    // little-endian int
    case 'L':
      {
      int size = (int)(argString[pos++]) - (int)'0';
      int value = ifs.readInt(size,
                              EndiannessTypes::little,
                              SignednessTypes::nosign);
      addIntArg(value, size);
      }
      break;
    // big-endian int
    case 'B':
      {
      int size = (int)(argString[pos++]) - (int)'0';
      int value = ifs.readInt(size,
                              EndiannessTypes::big,
                              SignednessTypes::nosign);
      addIntArg(value, size);
      }
      break;
/*    case '1':
      {
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_int8;
      arg.intValue_ = ifs.readu8();
      args_.push_back(arg);
      }
      break;
    case '2':
      {
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_int16;
      arg.intValue_ = ifs.readu16be();
      args_.push_back(arg);
      }
      break;
    case '4':
      {
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_int32;
      arg.intValue_ = ifs.readu32be();
      args_.push_back(arg);
      }
      break; */
    // label (name)
    case 'N':
    {
      char subtype = argString[pos++];
      
      if (subtype == 'I') {
        int labelIndex = ifs.readu8();
        if (labelIndex >= env.labels.size()) {
          throw TGenericException(T_SRCANDLINE,
                          "DandyScriptArgList::read()",
                          std::string("Out-of-range label index ref: ")
                            + "referred to label "
                            + TStringConversion::intToString(labelIndex)
                            + " with only "
                            + TStringConversion::intToString(env.labels.size())
                            + " labels available");
        }
        
        DandyScriptLabel label = env.labels[labelIndex];
        
        DandyScriptArg arg;
        arg.type_ = DandyScriptArg::type_labelIndexReference;
        arg.stringValue_ = label.name;
        args_.push_back(arg);
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                        "DandyScriptArgList::read()",
                        std::string("Unknown label ref subtype symbol: '")
                          + c
                          + "'");
      }
      
    }
      break;
    // game text string (looks similar to sjis+ascii, but characters
    // are actually encoded against a file-specific character sheet)
    case 'T':
    case 't':
      {
      int srcPos = ifs.tell();
      
      // save token with offset of string.
      // this can be used to look up the contents for assembly later,
      // from a separate string file.
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_gameTextString;
      arg.intValue_ = ifs.tell();
      args_.push_back(arg);
      
      // skip over the source string.
      // anything with bit 7 set is a 2-byte sequence;
      // otherwise, standard ascii.
      // terminated with null.
      while (!ifs.eof()) {
        unsigned char next = ifs.get();
        if (next == 0) break;
        else if ((next & 0x80) != 0) ifs.get();
      }
      }
      break;
    // c-string
    case 'S':
    case 's':
      {
      std::string str;
      while (!ifs.eof()) {
        char next = ifs.get();
        if (next == 0x00) break;
        
        // escape non-printable chars
        if (next < 0x20) {
          str += "\\x";
          std::string numstr = TStringConversion::intToString(next,
                   TStringConversion::baseHex).substr(2, std::string::npos);
          while (numstr.size() < 2) numstr = "0" + numstr;
          str += numstr;
        }
        // escape literal backslashes
        else if (next == '\\') {
          str += "\\\\";
        }
        // escape literal quotation marks
        else if (next == '"') {
          str += "\\\"";
        }
        else {
          str += next;
        }
      }
      
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_cstring;
      arg.stringValue_ = str;
      args_.push_back(arg);
      }
      break;
    // used to explicitly mark empty arglists
    case 'x':
      
      break;
    // ignored
    case ' ':
      
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "DandyScriptArgList::read()",
                              std::string("Unknown arg symbol: '")
                                + c
                                + "'");
      break;
    }
  }
}

void DandyScriptArgList::write(DandyScriptAsmEnv& env,
            BlackT::TStream& ofs, std::string argString) {
/*  unsigned int pos = 0;
  while (pos < argString.size()) {
    char c = argString[pos++];
    switch (c) {
    // string
    case 's':
      
      break;
    // label
    case 'l':
      
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "DandyScriptArgList::write()",
                              std::string("Unknown arg symbol: '")
                                + c
                                + "'");
      break;
    }
  } */
  
  for (ScriptArgCollection::iterator it = args_.begin();
       it != args_.end();
       ++it) {
    it->write(env, ofs);
  }
}

void DandyScriptArgList::save(DandyScriptEnv& env,
          std::ostream& ofs, std::string argString) {
  int num = 0;
  for (ScriptArgCollection::iterator it = args_.begin();
       it != args_.end();
       ++it) {
    it->print(env, ofs);
    
    if (num != (args_.size() - 1))
      ofs << " ";
    ++num;
  }
}

void DandyScriptArgList::load(DandyScriptAsmEnv& env,
          BlackT::TStream& ifs, std::string argString) {
  unsigned int pos = 0;
  while (pos < argString.size()) {
    char c = argString[pos++];
    switch (c) {
    // int
    case 'L':
    case 'B':
      {
      int size = (int)(argString[pos++]) - (int)'0';
      
//      int value = TParse::matchInt(ifs);
      int value = 0;
      // check for special references
      if (TParse::checkChar(ifs, '<')) {
        SpecialRefArg ref;
        ref.match(ifs);
        
        if (ref.name.size() <= 0) {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptArgList::load()",
                                  std::string("Unnamed int reference"));
        }
        
        // width in 16x16 blocks (only for one-line strings)
        if (ref.type.compare("CELLWIDTH") == 0) {
          int rawWidth = 0;
          if (isdigit(ref.name[0])) {
            int num = TStringConversion::stringToInt(ref.name);
            rawWidth = env.strings.at(num).width;
          }
          else {
            rawWidth = env.commonStrings.at(ref.name).width;
          }
          
          value = rawWidth / 16;
          if ((rawWidth % 16) != 0) ++value;
        }
        // width in 16x16 blocks, accounting for necessary padding
        // needed to function correctly with menu option highlighting
/*        if (ref.type.compare("SAFECELLWIDTH") == 0) {
          int rawWidth = 0;
          DandyScriptReader::ResultString* resultString = NULL;
          if (isdigit(ref.name[0])) {
            int num = TStringConversion::stringToInt(ref.name);
            resultString = &(env.strings.at(num));
            rawWidth = resultString->width;
          }
          else {
//            rawWidth = env.commonStrings.at(ref.name).width;
            resultString = &(env.commonStrings.at(ref.name));
            rawWidth = resultString->width;
          }
          
          // append padding to the cell if not already done
          if (!resultString->autoPadded) {
            int cellWidth = rawWidth / 16;
            if ((rawWidth % 16) != 0) ++cellWidth;
            
            int highlightWidth = cellWidth * 14;
            while (highlightWidth < rawWidth) {
              // MEGAHACK
              
              // 14px space
              resultString->str += 0xAD;
              // 1px space
              resultString->str += 0xAC;
              // 1px space
              resultString->str += 0xAC;
              
              ++cellWidth;
              highlightWidth += 14;
            }
            
            value = cellWidth;
            
            resultString->autoPadded = true;
          }
          else {
            int cellWidth = rawWidth / 16;
            if ((rawWidth % 16) != 0) ++cellWidth;
            value = cellWidth;
          }
        } */
        // width in 14x14 blocks (only for one-line strings)
        else if (ref.type.compare("SUBCELLWIDTH") == 0) {
          int rawWidth = 0;
          if (isdigit(ref.name[0])) {
            int num = TStringConversion::stringToInt(ref.name);
            rawWidth = env.strings.at(num).width;
          }
          else {
            rawWidth = env.commonStrings.at(ref.name).width;
          }
          
          value = rawWidth / 14;
          if ((rawWidth % 14) != 0) ++value;
        }
        else {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptArgList::load()",
                                  std::string("Unknown int reference type:")
                                  + ref.type);
        }
      }
      else {
        value = TParse::matchInt(ifs);
      }
      
      addIntArg(value, size);
      }
      break;
    // label (name)
    case 'N':
    {
      char subtype = argString[pos++];
      
      if (subtype == 'I') {
        TParse::matchChar(ifs, '&');
        
        std::string name = TParse::matchString(ifs);
        
        DandyScriptArg arg;
        arg.type_ = DandyScriptArg::type_labelIndexReference;
        arg.stringValue_ = name;
        args_.push_back(arg);
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                        "DandyScriptArgList::load()",
                        std::string("Unknown label ref subtype symbol: '")
                          + c
                          + "'");
      }
      
    }
      break;
    // game text string (looks similar to sjis+ascii, but characters
    // are actually encoded against a file-specific character sheet)
    case 'T':
    case 't':
      {
      bool isCommon = false;
      int offset = -1;
      std::string name;
      
/*      TParse::matchChar(ifs, '<');
      if (TParse::checkChar(ifs, 'C')) {
        TParse::matchChar(ifs, 'C');
        TParse::matchChar(ifs, 'O');
        TParse::matchChar(ifs, 'M');
        TParse::matchChar(ifs, 'M');
        TParse::matchChar(ifs, 'O');
        TParse::matchChar(ifs, 'N');
        TParse::matchChar(ifs, '_');
        
        while (!ifs.eof() && (ifs.peek() != '>')) {
          name += ifs.get();
        }
        
        isCommon = true;
      }
      else {
        TParse::matchChar(ifs, 'T');
        TParse::matchChar(ifs, 'E');
        TParse::matchChar(ifs, 'X');
        TParse::matchChar(ifs, 'T');
        TParse::matchChar(ifs, '_');
        offset = TParse::matchInt(ifs);
      }
      TParse::matchChar(ifs, '>'); */
      
      SpecialRefArg ref;
      ref.match(ifs);
      
      if (ref.type.compare("COMMON") == 0) {
        name = ref.name;
        isCommon = true;
      }
      else if (ref.type.compare("TEXT") == 0) {
        offset = TStringConversion::stringToInt(ref.name);
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "DandyScriptArgList::load()",
                                std::string("Unknown text reference type:")
                                + ref.type);
      }
      
      // save token with offset of string.
      // this can be used to look up the contents for assembly later,
      // from a separate string file.
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_gameTextString;
      arg.intValue_ = offset;
      arg.stringValue_ = name;
      args_.push_back(arg);
      }
      break;
    // c-string
    case 'S':
    case 's':
      {
      TParse::matchChar(ifs, '"');
      std::string str;
      while (!ifs.eof() && (ifs.peek() != '"')) {
        char next = ifs.get();
        
        if (next == '\\') {
          if (ifs.peek() == 'x') {
            std::string numstr = "0x";
            numstr += ifs.get();
            numstr += ifs.get();
            int num = TStringConversion::stringToInt(numstr);
            str += (char)num;
          }
          else {
            str += ifs.get();
          }
        }
        else {
          str += next;
        }
      }
      TParse::matchChar(ifs, '"');
      
      DandyScriptArg arg;
      arg.type_ = DandyScriptArg::type_cstring;
      arg.stringValue_ = str;
      args_.push_back(arg);
      }
      break;
    // used to explicitly mark empty arglists
    case 'x':
      
      break;
    // ignored
    case ' ':
      
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "DandyScriptArgList::load()",
                              std::string("Unknown arg symbol: '")
                                + c
                                + "'");
      break;
    }
  }
}

void DandyScriptArgList::addIntArg(int value, int size) {
  DandyScriptArg arg;
  arg.intValue_ = value;
  
  switch (size) {
  case 1:
    arg.type_ = DandyScriptArg::type_int8;
    break;
  case 2:
    arg.type_ = DandyScriptArg::type_int16;
    break;
  case 3:
    arg.type_ = DandyScriptArg::type_int24;
    break;
  case 4:
    arg.type_ = DandyScriptArg::type_int32;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptArgList::addIntArg()",
                            std::string("Unsupported int size: ")
                              + TStringConversion::intToString(size));
    break;
  }
  
  args_.push_back(arg);
}

void DandyScriptArgList::SpecialRefArg::match(BlackT::TStream& ifs) {
  type = "";
  name = "";

  TParse::matchChar(ifs, '<');
  
  while (!ifs.eof() && (ifs.peek() != '_')) {
    type += ifs.get();
  }

  TParse::matchChar(ifs, '_');
  
  while (!ifs.eof() && (ifs.peek() != '>')) {
    name += ifs.get();
  }

  TParse::matchChar(ifs, '>');
}


}
