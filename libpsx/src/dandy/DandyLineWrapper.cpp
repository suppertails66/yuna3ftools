#include "dandy/DandyLineWrapper.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

/*const static int controlOpsStart = 0xF0;
const static int controlOpsEnd   = 0x100;

const static int code_space   = 0x20;
const static int code_arwyn   = 0xF2;
const static int code_party1  = 0xF3;
const static int code_party2  = 0xF4;
const static int code_wait    = 0xF5;
const static int code_5spaces = 0xF6;
const static int code_scrollout = 0xF7;
const static int code_br      = 0xF8;
const static int code_dict0   = 0xF9;
const static int code_dict1   = 0xFA;
const static int code_dict2   = 0xFB;
const static int code_dict3   = 0xFC;
const static int code_buf     = 0xFD;
const static int code_end     = 0xFE;
const static int code_waitend = 0xFF; */

const static int controlCodeStartChar = 0x5C;
const static int controlCodeStartSeq = 0x5C00;

const static int code_color     = 0x5C40; // 2b param
const static int code_b         = 0x5C62; // not used
const static int code_clear     = 0x5C63;
const static int code_m         = 0x5C6D; // 2b param
const static int code_br        = 0x5C6E;
const static int code_pause     = 0x5C70; // 2b param
const static int code_q         = 0x5C71;
const static int code_speed     = 0x5C73; // 2b param
const static int code_t         = 0x5C74;
const static int code_voice     = 0x5C76; // 2b param
const static int code_wait      = 0x5C77;

const static int code_space     = 0x5F;

const static int code_fwspace = 0xAA;
const static int code_space6px = 0xAB;
const static int code_space1px = 0xAC;
const static int code_space14px = 0xAD;

// added for translation
//const static int code_tilebr  = 0xF0;

DandyLineWrapper::DandyLineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                int xSize__,
                int ySize__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    sizeTable(sizeTable__),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_single),
    errorOnOverflow(false),
    alignMode(align_none),
//    padMode(pad_off)
    padWidth(-1),
    autoPadMode(autopad_none) {
  
}

int DandyLineWrapper::widthOfKey(int key) {
/*  if ((key == code_br)) return 0;
  else if ((key == code_color)) return 0;
  else if ((key == code_end)) return 0;
  else if ((key == code_wait)) return 0;
  else if ((key == code_waitend)) return 0;
//  else if ((key == code_tilebr)) return 16;  // assume worst case
  else if ((key >= controlOpsStart) && (key < controlOpsEnd)) return 0; */
  if ((key & 0xFF00) == controlCodeStartSeq) return 0;
  
  return sizeTable[key];
}

int DandyLineWrapper::advanceWidthOfKey(int key) {
  return widthOfKey(key);
}

bool DandyLineWrapper::isWordDivider(int key) {
  if (
      (key == code_br)
      || (key == code_space)
     ) return true;
  
  return false;
}

bool DandyLineWrapper::isLinebreak(int key) {
  if (
      (key == code_br)
      ) return true;
  
  return false;
}

bool DandyLineWrapper::isBoxClear(int key) {
  // END
  if ((key == code_clear)
      || (key == code_wait)) return true;
  
  return false;
}

void DandyLineWrapper::onBoxFull() {
/*  if (clearMode == clearMode_default) {
    std::string content;
    if (lineHasContent) {
      // wait
      content += thingy.getEntry(code_wait);
      content += thingy.getEntry(code_br);
      currentScriptBuffer.write(content.c_str(), content.size());
    }
    // linebreak
    stripCurrentPreDividers();
    
    currentScriptBuffer.put('\n');
    xPos = 0;
    yPos = 0;
  }
  else if (clearMode == clearMode_messageSplit) {
    std::string content;
//      if (lineHasContent) {
      // wait
//        content += thingy.getEntry(code_wait);
//        content += thingy.getEntry(code_br);
      content += thingy.getEntry(code_end);
      content += "\n\n#ENDMSG()\n\n";
      currentScriptBuffer.write(content.c_str(), content.size());
//      }
    // linebreak
    stripCurrentPreDividers();
    
    xPos = 0;
    yPos = 0;
  } */
  
  std::string content;
  if (lineHasContent) {
    // wait
    content += thingy.getEntry(code_wait);
//    content += thingy.getEntry(code_br);
//    content += linebreakString();
    content += thingy.getEntry(code_clear);
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  xPos = 0;
  yPos = -1;

/*  std::cerr << "WARNING: line " << lineNum << ":" << std::endl;
  std::cerr << "  overflow at: " << std::endl;
  std::cerr << streamAsString(currentScriptBuffer)
    << std::endl
    << streamAsString(currentWordBuffer) << std::endl; */
}

//int DandyLineWrapper::linebreakKey() {
//  return code_br;
//}

std::string DandyLineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(code_br);
  if (breakMode == breakMode_single) {
    return breakString;
  }
  else {
    return breakString + breakString;
  }
}

//int DandyLineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_single) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void DandyLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
/*  if (isLinebreak(key)) {
    if ((yPos != -1) && (yPos >= ySize - 1)) {
      flushActiveWord();
      
    }
  } */
}

void DandyLineWrapper::afterSymbolAdded(BlackT::TStream& ifs, int key) {
  // skip argument to parameterized control codes
  if ((key == code_color)
      || (key == code_m)
      || (key == code_pause)
      || (key == code_speed)
      || (key == code_voice)) {
//    ifs.seekoff(2);
    
    // FIXME: \p does not take fixed 2-byte parameter.
    // should instead read characters until non-digit encountered
    char first = ifs.get();
    char second = ifs.get();
    
/*    std::string firstStr;
    std::string secondStr;
    
    firstStr = firstStr + "<$"
      + TStringConversion::intToString(first, TStringConversion::baseHex)
          .substr(2, std::string::npos)
      + ">";
    secondStr = secondStr + "<$"
      + TStringConversion::intToString(second, TStringConversion::baseHex)
          .substr(2, std::string::npos)
      + ">";
    
    currentWordBuffer.write(firstStr.c_str(), firstStr.size());
    currentWordBuffer.write(secondStr.c_str(), secondStr.size()); */
    
    currentWordBuffer.put(first);
    currentWordBuffer.put(second);
  }
  
}

void DandyLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != code_br) || (breakMode == breakMode_single)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void DandyLineWrapper::beforeLinebreak(
    LinebreakSource clearSrc, int key) {
  if (errorOnOverflow) {
    if ((clearSrc == linebreakLineFull)
        || (clearSrc == linebreakBoxEnd)) {
        throw TGenericException(T_SRCANDLINE,
                                "DandyLineWrapper::beforeLinebreak()",
                                std::string("Line too long, and error ")
                                + "on overflow enabled\n"
                                + "At error, x-pos was: "
                                + TStringConversion::intToString(
                                  xPos + currentWordWidth)
                                + "/"
                                + TStringConversion::intToString(
                                  xSize));
    }
  }
}

void DandyLineWrapper::afterLinebreak(
    LinebreakSource clearSrc, int key) {
/*  if (clearSrc != linebreakBoxEnd) {
    if (spkrOn) {
      xPos = spkrLineInitialX;
    }
  } */
  
/*  if (clearSrc == linebreakManual) {
    if (breakMode == breakMode_double) {
      --yPos;
    }
  } */
}

void DandyLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
  if (errorOnOverflow) {
    if ((clearSrc == boxClearBoxFull)) {
        throw TGenericException(T_SRCANDLINE,
                                "DandyLineWrapper::beforeBoxClear()",
                                std::string("Line too long, and error ")
                                + "on overflow enabled\n"
                                + "At error, x-pos was: "
                                + TStringConversion::intToString(
                                  xPos + currentWordWidth)
                                + "/"
                                + TStringConversion::intToString(
                                  xSize));
    }
  }
  
  if (((clearSrc == boxClearManual) && (key == code_wait))) {
    xBeforeWait = xPos;
  }
}

void DandyLineWrapper::afterBoxClear(
  BoxClearSource clearSrc, int key) {
  // wait pauses but does not automatically break the line
  if (((clearSrc == boxClearManual) && (key == code_wait))) {
    xPos = xBeforeWait;
    yPos = -1;
/*    if (breakMode == breakMode_single) {
      yPos = -1;
    }
    else {
      yPos = -2;
    } */
  }
}

bool DandyLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("SETBREAKMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if (type.compare("SINGLE") == 0) {
      breakMode = breakMode_single;
    }
    else if (type.compare("DOUBLE") == 0) {
      breakMode = breakMode_double;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "DandyLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown break mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
/*  else if (name.compare("PARABR") == 0) {
//    if (yPos >= ySize) {
//      onBoxFull();
//    }
//    else {
//      onBoxFull();
//    }
    flushActiveWord();
    outputLinebreak();
    return true;
  } */
  else if (name.compare("ENDSTRING") == 0) {
    // HACK: do alignment calculations
    alignCurrentScript();
    padCurrentScript();
    
    currentScriptBuffer.writeString(
      std::string("\n//last xpos: ")
      + TStringConversion::intToString(xPos));
    currentScriptBuffer.writeString(
      std::string("\n#SETLASTXPOS(")
      + TStringConversion::intToString(xPos)
      + ")");
    
    processEndMsg(ifs);
    return true;
  }
  else if (name.compare("SETERRORONOVERFLOW") == 0) {
    int value = TParse::matchInt(ifs);
    errorOnOverflow = (value != 0);
    return true;
  }
  else if (name.compare("SETALIGNMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if ((type.compare("NONE") == 0)) {
      alignMode = align_none;
    }
    else if (type.compare("LEFT") == 0) {
      alignMode = align_left;
    }
    else if (type.compare("CENTER") == 0) {
      alignMode = align_center;
    }
    else if (type.compare("RIGHT") == 0) {
      alignMode = align_right;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "DandyLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown align mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
  else if (name.compare("SETPADWIDTH") == 0) {
/*    std::string type = TParse::matchName(ifs);
    
    if ((type.compare("OFF") == 0)) {
      padMode = pad_off;
    }
    else if (type.compare("ON") == 0) {
      padMode = pad_on;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "DandyLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown pad mode '"
                                + type
                                + "'");
    } */
    
    padWidth = TParse::matchInt(ifs);
    
    return true;
  }
  else if (name.compare("SETAUTOPADMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if ((type.compare("NONE") == 0)) {
      autoPadMode = autopad_none;
    }
    else if (type.compare("MENU") == 0) {
      autoPadMode = autopad_menu;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "DandyLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown autopad mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
  
  return false;
}

void DandyLineWrapper::padCurrentScript() {
  flushActiveWord();
  
/*  switch (padMode) {
  case pad_off:
    return;
    break;
  case pad_on:
  {
    int width = (xSize - xPos);
    std::string widthStr = generatePaddingString(width);
    
    TBufStream newscr;
    
    findContentStart(currentScriptBuffer);
    int contentStartPos = currentScriptBuffer.tell();
    currentScriptBuffer.seek(0);
    newscr.writeFrom(currentScriptBuffer, contentStartPos);
    
    newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
    
    newscr.writeString(widthStr);
    
    currentScriptBuffer = newscr;
    xPos = xSize;
  }
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyLineWrapper::padCurrentScript()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": unknown pad mode");
    break;
  } */
  
  if (autoPadMode == autopad_menu) {
    int rawWidth = xPos;
    int cellWidth = rawWidth / 16;
    if ((rawWidth % 16) != 0) ++cellWidth;
    
    int highlightWidth = cellWidth * 14;
    
    if (highlightWidth < rawWidth) {
      int targetPad = 0;
      
      // round up to next multiple of 16
//      if ((rawWidth % 16) != 0) {
//        int next = (16 - (rawWidth % 16));
//        targetPad += next;
//        rawWidth += next;
//      }
      
      int additionalCellsNeeded = (rawWidth - highlightWidth) / 14;
      if (((rawWidth - highlightWidth) % 14) != 0) ++additionalCellsNeeded;
      
      targetPad += (additionalCellsNeeded * 16);
      
      std::string widthStr = generatePaddingString(targetPad);
      
      TBufStream newscr;
      
      findContentStart(currentScriptBuffer);
      int contentStartPos = currentScriptBuffer.tell();
      currentScriptBuffer.seek(0);
      newscr.writeFrom(currentScriptBuffer, contentStartPos);
      
      newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
      
      newscr.writeString(widthStr);
      
      currentScriptBuffer = newscr;
      xPos += targetPad;
    }
  }
  
  if (padWidth > 0) {
    int width = (padWidth - xPos);
    std::string widthStr = generatePaddingString(width);
    
    TBufStream newscr;
    
    findContentStart(currentScriptBuffer);
    int contentStartPos = currentScriptBuffer.tell();
    currentScriptBuffer.seek(0);
    newscr.writeFrom(currentScriptBuffer, contentStartPos);
    
    newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
    
    newscr.writeString(widthStr);
    
    currentScriptBuffer = newscr;
    xPos = padWidth;
  }
}

void DandyLineWrapper::findContentStart(BlackT::TStream& ifs) {
  // hacks on top of hacks
/*  while ((ifs.peek() == '\r') || (ifs.peek() == '\n')) ifs.get();
  while (!ifs.eof()
         && (ifs.peek() == '#')) {
    while (!ifs.eof()
         && (ifs.get() != '\n'));
    while ((ifs.peek() == '\r') || (ifs.peek() == '\n')) ifs.get();
//    TParse::skipSpace(ifs);
  } */
  
  while (ifs.tell() > 0) {
    ifs.seekoff(-1);
    if (ifs.peek() == '\n') {
      ifs.get();
      break;
    }
  }
}

void DandyLineWrapper::alignCurrentScript() {
  flushActiveWord();
  
  switch (alignMode) {
  case align_none:
    return;
    break;
  case align_left:
  {
    int width = (xSize - xPos);
    std::string widthStr = generatePaddingString(width);
    
    TBufStream newscr;
    
    findContentStart(currentScriptBuffer);
    int contentStartPos = currentScriptBuffer.tell();
    currentScriptBuffer.seek(0);
    newscr.writeFrom(currentScriptBuffer, contentStartPos);
    
    newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
    
    newscr.writeString(widthStr);
    
    currentScriptBuffer = newscr;
    xPos = xSize;
  }
    break;
  case align_center:
  {
    int remainingArea = (xSize - xPos);
    int width = remainingArea / 2;
    std::string widthStr = generatePaddingString(width);
    
    TBufStream newscr;
    
    findContentStart(currentScriptBuffer);
    int contentStartPos = currentScriptBuffer.tell();
    currentScriptBuffer.seek(0);
    newscr.writeFrom(currentScriptBuffer, contentStartPos);
  
    newscr.writeString(widthStr);
    
    newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
    
    // if remaining area is odd, make up the extra pixel of spacing
    // on the right
    if ((remainingArea % 2) != 0) {
      widthStr += thingy.getEntry(code_space1px);
    }
    
    newscr.writeString(widthStr);
    
    currentScriptBuffer = newscr;
    xPos = xSize;
  }
    break;
  case align_right:
  {
    int width = (xSize - xPos);
    std::string widthStr = generatePaddingString(width);
    
    TBufStream newscr;
    
    findContentStart(currentScriptBuffer);
    int contentStartPos = currentScriptBuffer.tell();
    currentScriptBuffer.seek(0);
    newscr.writeFrom(currentScriptBuffer, contentStartPos);
    
    newscr.writeString(widthStr);
    
    newscr.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
    
    currentScriptBuffer = newscr;
    xPos = xSize;
  }
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyLineWrapper::alignCurrentScript()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": unknown align mode");
    break;
  }
}

std::string DandyLineWrapper::generatePaddingString(int width) {
  std::string result;
  
  while (width > 0) {
    if (width >= 14) {
      result += thingy.getEntry(code_space14px);
      width -= 14;
    }
    else if (width >= 6) {
      result += thingy.getEntry(code_space6px);
      width -= 6;
    }
    else {
      result += thingy.getEntry(code_space1px);
      width -= 1;
    }
  }
  
  return result;
}

}
