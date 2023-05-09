#include "yuna3/Yuna3LineWrapper.h"
//#include "yuna3/Yuna3MsgConsts.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

//const static int controlOpsStart = 0x40;

const static int code_br4       = 0x0B;
//const static int code_forceBoxEnd = 0xEC;
//const static int code_forceBoxPause = 0xED;
//const static int code_clear    = 0xF2;
const static int code_br       = 0x0A;
const static int code_end      = 0x00;

//const static int code_clear    = 0x03;
//const static int code_end16    = 0x16;
//const static int code_end1A    = 0x1A;
const static int fontBase = 0x10;
const static int code_nbsp     = 0x1D;
const static int code_period   = 0x2E;
const static int code_exclaim  = 0x21;
const static int code_question = 0x3F;
const static int code_rquotesing = 0x7E;
const static int code_rquote   = 0x7D;
const static int code_hyphen   = 0x2D;
const static int code_ellipsis = 0x16;
const static int code_dash     = 0x15;
const static int code_space    = 0x20;
// TODO
const static int code_space1px = 0x20;
const static int code_space8px = 0x20;
const static int code_space4px = 0x20;

// TODO
const static int code_emphModeToggle = 0x5F;

Yuna3LineWrapper::Yuna3LineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                CharSizeTable sizeTableEmph__,
                KerningMatrix kerningMatrix__,
                KerningMatrix kerningMatrixEmph__,
                bool isNonstd__,
                int xSize__,
                int ySize__,
                int controlOpsStart__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    controlOpsStart(default_controlOpsStart),
    sizeTable(sizeTable__),
    sizeTableEmph(sizeTableEmph__),
    kerningMatrix(kerningMatrix__),
    kerningMatrixEmph(kerningMatrixEmph__),
    isNonstd(isNonstd__),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_single),
    failOnBoxOverflow(false),
    doubleWidthModeOn(false),
    doubleWidthCharsInWord(false),
    padAndCenter_on(false),
    padAndCenter_leftPad(0),
    padAndCenter_width(0),
    longestLineLen(0),
    emphModeOn(false),
    br4ModeOn(false),
    currentChar(-1),
    lastChar(-1),
    defaultKerningOffset(0) {
  
}

int Yuna3LineWrapper::getLocalBrId() const {
  // HACK
  // (br char differs between regular and cutscene fonts)
  if (br4ModeOn)
    return thingy.matchTableEntry("[br4]").id;
  else
    return thingy.matchTableEntry("\\n").id;
}

int Yuna3LineWrapper::getLocalSpaceId() const {
  // HACK
  return thingy.matchTableEntry(" ").id;
}

/*int Yuna3LineWrapper::getLocalEspId() const {
  // HACK
  return thingy.matchTableEntry("[esp]").id;
}*/

int Yuna3LineWrapper::widthOfKey(int key) {
  if ((key == getLocalBrId())) return 0;
//  else if ((key >= controlOpsStart)) return 0;
  else if ((key < fontBase)) return 0;
  
//  if (key == code_emphModeToggle) return 0;
  
//  std::cerr << std::hex << key << " " << std::dec << sizeTable[key] << " " << currentWordWidth << " " << xPos << std::endl;
//  char c;
//  std::cin >> c;
  
  int result = sizeTable[key].glyphWidth;
  
  if (emphModeOn
      && (key != getLocalSpaceId()))
    result = sizeTableEmph[key].glyphWidth;
  
  if (doubleWidthModeOn) result *= 2;
  return result;
}

int Yuna3LineWrapper::advanceWidthOfKey(int key) {
//  return widthOfKey(key);
  
  if ((key == getLocalBrId())) return 0;
  else if ((key < fontBase)) return 0;
  
  int result = sizeTable[key].advanceWidth;
  
  if (emphModeOn
      && (key != getLocalSpaceId()))
    result = sizeTableEmph[key].advanceWidth;
  
  if (doubleWidthModeOn) result *= 2;
  return result;
}

bool Yuna3LineWrapper::isWordDivider(int key) {
  if ((key == getLocalBrId())
      || (key == getLocalSpaceId()
      // TRANSLATION ONLY
//      || (key == Yuna3TextScript::op_13)
      )
     ) return true;
  
  return false;
}

bool Yuna3LineWrapper::isVisibleWordDivider(int key, int nextKey) {
  if (isNonstd) return false;
  
  if ((key == code_hyphen)
      || (key == code_ellipsis)
      || (key == code_dash)) {
    // if followed by punctuation or non-breaking space,
    // do not treat as divider
    if ((nextKey == code_exclaim)
        || (nextKey == code_question)
        || (nextKey == code_rquote)
        || (nextKey == code_rquotesing)
        || (nextKey == code_nbsp)) {
      return false;
    }
    
    return true;
  }
  
  return false;
}

bool Yuna3LineWrapper::isLinebreak(int key) {
  if ((key == getLocalBrId())
      ) return true;
  
  return false;
}

bool Yuna3LineWrapper::isBoxClear(int key) {
/*  if (
//      (key == code_end16)
//      || (key == code_end1A)
//      || 
      (key == code_clear)
    ) return true;*/
  
  return false;
}

void Yuna3LineWrapper::onBoxFull() {
  // TODO?
  std::string content;
//  if (lineHasContent) {
    // full box + linebreak = automatic box wait
    content += thingy.getEntry(getLocalBrId());
    
    currentScriptBuffer.write(content.c_str(), content.size());
//  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  currentScriptBuffer.put('\n');
  xPos = 0;
//  yPos = -1;
  // ???
  yPos = 0;
}

//int Yuna3LineWrapper::linebreakKey() {
//  return code_br;
//}

std::string Yuna3LineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(getLocalBrId());
  if (breakMode == breakMode_single) {
    return breakString;
  }
  else {
    return breakString + breakString;
  }
}

//int Yuna3LineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_single) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void Yuna3LineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
  if (key >= fontBase) {
    // if a literal, update current/last char fields used for kerning
    // never set lastChar to space, to allow for inter-word kerning
//    if (currentChar != getLocalSpaceId())
      lastChar = currentChar;
    currentChar = key;
    
    // TODO: if lastChar is not -1, update pendingAdvanceWidth based on
    // kerning between new character and previous
    if ((lastChar != -1)
        && (lastChar != getLocalSpaceId())) {
      int offset = kerningMatrix.data(currentChar - fontBase, lastChar - fontBase);
      
      if (emphModeOn)
        offset = kerningMatrixEmph.data(currentChar - fontBase, lastChar - fontBase);
      
      // HACK: default offset
      if (offset == 0) offset = defaultKerningOffset;
      
//      std::cerr << (char)lastChar << " " << (char)currentChar
//        << " " << xPos << " " << currentWordWidth
//        << " " << pendingAdvanceWidth << " " << offset << std::endl;
      
      pendingAdvanceWidth += offset;
    }
  }
  
  if (key == code_emphModeToggle) {
    emphModeOn = !emphModeOn;
  }
}

void Yuna3LineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != getLocalBrId()) || (breakMode == breakMode_single)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void Yuna3LineWrapper::onLineContentStarted() {
  lineContentStartOffsets.push_back(currentScriptBuffer.tell());
}

void Yuna3LineWrapper::beforeLinebreak(
    LinebreakSource clearSrc, int key) {
  if (clearSrc == linebreakManual) {
    if ((key != code_br4)
        || br4ModeOn) {
      applyPadAndCenterToCurrentLine();
      doLineEndPadChecks();
    }
    
    if (key == code_br4)
      br4ModeOn = true;
  }
  
  // reset kerning char fields
  lastChar = -1;
  currentChar = -1;
}

void Yuna3LineWrapper::afterLinebreak(
    LinebreakSource clearSrc, int key) {
  // the double-width text modes can't have text automatically broken
  // because there aren't enough lines in the box for it,
  // so it's an immediate error
  if (
//      doubleWidthModeOn
      doubleWidthCharsInWord
//    && (clearSrc != linebreakManual)
      ) {
    throw TGenericException(T_SRCANDLINE,
                            "Yuna3LineWrapper::afterLinebreak()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": double-width mode box overflow");
  }
}

void Yuna3LineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
  if (failOnBoxOverflow && (clearSrc != boxClearManual)) {
    throw TGenericException(T_SRCANDLINE,
                            "Yuna3LineWrapper::beforeBoxClear()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ": box overflow");
  }
  
  doLineEndPadChecks();
  
  // reset kerning char fields
  lastChar = -1;
  currentChar = -1;
}

void Yuna3LineWrapper::afterBoxClear(
    BoxClearSource clearSrc, int key) {
  
}

void Yuna3LineWrapper::beforeWordFlushed() {
  doubleWidthCharsInWord = false;
}

char Yuna3LineWrapper::literalOpenSymbol() const {
  return myLiteralOpenSymbol;
}

char Yuna3LineWrapper::literalCloseSymbol() const {
  return myLiteralCloseSymbol;
}

bool Yuna3LineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("STARTSTRING") == 0) {
    return false;
  }
  else if (name.compare("ENDSTRING") == 0) {
    // HACK: for pad+center.
    // i don't remember how much of this is actually necessary,
    // this is pure copy and paste from another file
    // FIXME
      // output current word
      flushActiveWord();
      
      doLineEndPadChecks();
      applyPadAndCenterToCurrentLine();
//      applyPadAndCenterToCurrentBlock();
//      padAndCenter_on = false;
      
      beforeBoxClear(boxClearManual, code_end);
        
      xPos = 0;
      yPos = 0;
      pendingAdvanceWidth = 0;
      
      lineContentStartOffsets.clear();
      longestLineLen = 0;
      
      afterBoxClear(boxClearManual, code_end);
      
      lineHasContent = false;
      currentLineContentStartInBuffer = -1;
    
    br4ModeOn = false;
    processEndMsg(ifs);
    return true;
  }
  else if (name.compare("SETBREAKMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if (type.compare("SINGLE") == 0) {
      breakMode = breakMode_single;
    }
    else if (type.compare("DOUBLE") == 0) {
      breakMode = breakMode_double;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "Yuna3LineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown break mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
  else if (name.compare("SETFAILONBOXOVERFLOW") == 0) {
    int value = TParse::matchInt(ifs);
    failOnBoxOverflow = (value != 0);
    
    return true;
  }
  else if (name.compare("SETPADANDCENTER") == 0) {
    padAndCenter_leftPad = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    padAndCenter_width = TParse::matchInt(ifs);
    
    padAndCenter_on = true;
    
    return true;
  }
  else if (name.compare("DISABLEPADANDCENTER") == 0) {
    // output current word
    flushActiveWord();
    applyPadAndCenterToCurrentLine();
    padAndCenter_on = false;
    
    return true;
  }
  else if ((name.compare("BREAKBOX") == 0)) {
/*    flushActiveWord();
    int count = ySize - yPos;
    for (int i = 0; i < count; i++) {
      outputLinebreak();
    } */
    
    int targetSize = ySize - 1;
    
    if (ySize == -1) {
      throw TGenericException(T_SRCANDLINE,
                              "Yuna3LineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break infinitely-high box");
    }
    
    flushActiveWord();
    std::string breakstr = thingy.getEntry(getLocalBrId());
    while (yPos < targetSize) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
    
    onBoxFull();
    
    return true;
  }
  else if (name.compare("BREAKGROUP") == 0) {
    if (yPos > ((ySize/2) - 1)) {
      throw TGenericException(T_SRCANDLINE,
                              "Yuna3LineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": tried to break overflowed group");
    }
      
    std::string breakstr = thingy.getEntry(getLocalBrId());
  
//    if (currentWordBuffer.size() <= 0) {
//      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
//    }
    
//    if (yPos != ((ySize/2) - 1)) {
//      std::cerr << "1: " << ySize << " " << yPos << std::endl;
    flushActiveWord();
//      std::cerr << "2: " << ySize << " " << yPos << std::endl;
    while (yPos < ((ySize/2) - 1)) {
//      while (yPos < ((ySize/2))) {
      currentScriptBuffer.write(breakstr.c_str(), breakstr.size());
      ++yPos;
    }
//      std::cerr << "3: " << ySize << " " << yPos << std::endl;
  
    // HACK to correctly handle empty groups
    lineHasContent = true;
    
    onBoxFull();
//      std::cerr << "4: " << ySize << " " << yPos << std::endl;
//    }
    
    return true;
  }
  else if (name.compare("SETDEFAULTKERNINGOFFSET") == 0) {
    defaultKerningOffset = TParse::matchInt(ifs);
    return true;
  }
  // HACK: reset position for subtitle commands
  else if ((name.compare("W") == 0)
           || (name.compare("OFF") == 0)
           || (name.compare("WOFF") == 0)
           || (name.compare("VRAMOVERRIDE") == 0)) {
    flushActiveWord();
    xPos = 0;
    yPos = 0;
    return false;
  }
  
  return false;
}

std::string Yuna3LineWrapper::getPadString(int width) {
  std::string output;
  if (width <= 0) return output;
  
  while (width > 0) {
    if (width >= 8) {
      output += thingy.getEntry(code_space8px);
      width -= 8;
    }
    else if (width >= 4) {
      output += thingy.getEntry(code_space4px);
      width -= 4;
    }
    else {
      output += thingy.getEntry(code_space1px);
      width -= 1;
    }
  }
  
/*  output += thingy.getEntry(Yuna3TextScript::op_spaces);
  output += "<$";
  std::string valstr
    = TStringConversion::intToString(width, TStringConversion::baseHex)
        .substr(2, std::string::npos);
  while (valstr.size() < 2) valstr = std::string("0") + valstr;
  output += valstr;
  output += ">";*/
  return output;
}

void Yuna3LineWrapper::doLineEndPadChecks() {
  if (xPos > longestLineLen) longestLineLen = xPos;
}

void Yuna3LineWrapper::applyPadAndCenterToCurrentLine() {
  if (!padAndCenter_on) return;
  if (!lineHasContent) return;

  TBufStream temp;
  
  // copy script buffer up to start of current line content
  currentScriptBuffer.seek(0);
  temp.writeFrom(currentScriptBuffer, currentLineContentStartInBuffer);
  
  // insert padding at start of current line
//  std::cerr << "x: " << xPos << std::endl;
//  std::cerr << "padAndCenter_width: " << padAndCenter_width << std::endl;
  int centerOffset = 0;
  if (padAndCenter_width > 0) {
    centerOffset = (padAndCenter_width - xPos) / 2;
//    if (centerOffset < padAndCenter_leftPad) {
    if (centerOffset < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "Yuna3LineWrapper::applyPadAndCenterToCurrentLine()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": cannot fit on line");
    }
  }
//  std::cerr << "centerOffset: " << centerOffset << std::endl;
//  char c;
//  std::cin >> c;
  temp.writeString(getPadString(padAndCenter_leftPad + centerOffset));
  
  // copy remaining content from original script buffer
  if (currentScriptBuffer.remaining() > 0)
    temp.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
  
  currentScriptBuffer = temp;
}

void Yuna3LineWrapper::applyPadAndCenterToCurrentBlock() {
  if (!padAndCenter_on) return;
  if (longestLineLen <= 0) return;
  if (lineContentStartOffsets.size() <= 0) return;

  TBufStream temp;
  
//  std::cerr << longestLineLen << std::endl;
  
  int centerOffset = 0;
  if (padAndCenter_width > 0) {
    centerOffset = (padAndCenter_width - longestLineLen) / 2;
    if ((centerOffset < 0)
        || (longestLineLen > padAndCenter_width)) {
      throw TGenericException(T_SRCANDLINE,
                              "Yuna3LineWrapper::applyPadAndCenterToCurrentBlock()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": cannot fit on line");
    }
  }
  
  // copy script buffer up to start of first line
  currentScriptBuffer.seek(0);
  temp.writeFrom(currentScriptBuffer, lineContentStartOffsets[0]);
  
  std::string padString = getPadString(padAndCenter_leftPad + centerOffset);
//  std::cerr << padString << std::endl;
  // awkwardly splice all subsequent lines together with padding applied
  for (int i = 0; i < lineContentStartOffsets.size() - 1; i++) {
    temp.writeString(padString);
    temp.writeFrom(currentScriptBuffer,
      lineContentStartOffsets[i + 1] - lineContentStartOffsets[i]);
  }
  
  // remaining content
  temp.writeString(padString);
  if (currentScriptBuffer.remaining() > 0)
    temp.writeFrom(currentScriptBuffer, currentScriptBuffer.remaining());
  
  currentScriptBuffer = temp;
}

}
