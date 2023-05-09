#include "util/TBitmapFont.h"
#include "util/TPngConversion.h"
#include "util/TBufStream.h"
#include "util/TIniFile.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"

namespace BlackT {


static std::string asHex(int num, int padding) {
  std::string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, std::string::npos);
  while (str.size() < padding) str = std::string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

/*static std::string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

static std::string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
} */

static bool getIntIfExists(int* dst, const TIniFile& ini, std::string section,
                           std::string key) {
  if (dst == NULL) return false;
  if (!ini.hasSection(section)) return false;
  if (!ini.hasKey(section, key)) return false;
  
  *dst = TStringConversion::stringToInt(ini.valueOfKey(section, key));
  return true;
}

static bool getBoolIfExists(bool* dst, const TIniFile& ini, std::string section,
                     std::string key) {
  if (dst == NULL) return false;
  if (!ini.hasSection(section)) return false;
  if (!ini.hasKey(section, key)) return false;
  
  *dst = (TStringConversion::stringToInt(ini.valueOfKey(section, key)) != 0);
  return true;
}



TBitmapFont::TBitmapFont()
  : charH(0),
//    vShiftRange(0),
    minVShift(0),
    maxVShift(0),
    autoKerningMode(autokern_square),
    autoKerningOffset(0) { }

void TBitmapFont::load(std::string inprefix) {
  
  TIniFile index;
  index.readFile(inprefix + "index.txt");
  
  TGraphic sheet;
  TPngConversion::RGBAPngToGraphic(inprefix + "sheet.png", sheet);
  
  int numChars = TStringConversion::stringToInt(
    index.valueOfKey("Properties", "numChars"));
  int charsPerRow = TStringConversion::stringToInt(
    index.valueOfKey("Properties", "charsPerRow"));
  int gridW = TStringConversion::stringToInt(
    index.valueOfKey("Properties", "gridW"));
  int gridH = TStringConversion::stringToInt(
    index.valueOfKey("Properties", "gridH"));
  bool noKerning = false;
  if (index.hasKey("Properties", "noKerning")) {
     noKerning = TStringConversion::stringToInt(
      index.valueOfKey("Properties", "noKerning")) != 0;
  }
  autoKerningOffset = 0;
  if (index.hasKey("Properties", "autoKerningOffset")) {
     autoKerningOffset = TStringConversion::stringToInt(
      index.valueOfKey("Properties", "autoKerningOffset"));
  }
  int autoWidthOffset = 0;
  if (index.hasKey("Properties", "autoWidthOffset")) {
     autoWidthOffset = TStringConversion::stringToInt(
      index.valueOfKey("Properties", "autoWidthOffset"));
  }
  autoKerningMode = autokern_cross;
  if (index.hasKey("Properties", "autoKerningMode")) {
    std::string modeStr = index.valueOfKey("Properties", "autoKerningMode");
    if (modeStr.compare("cross") == 0) {
      autoKerningMode = autokern_cross;
    }
    else if (modeStr.compare("square") == 0) {
      autoKerningMode = autokern_square;
    }
  }
  
  charH = gridH;
  
//  FontCharCollection fontChars;
  
  // wastefully pre-read data just to get the vshift
//  int minVShift = 0;
//  int maxVShift = 0;
  for (int k = 0; k < numChars; k++) {
    std::string sectionNumStr = asHex(k, 4);
    std::string sectionName = std::string("char") + sectionNumStr;
    TBitmapFontChar data;
    data.readFromIni(index, sectionName);
    
    if (data.vShift < minVShift) minVShift = data.vShift;
    if (data.vShift > maxVShift) maxVShift = data.vShift;
  }
  
  int vShiftRange = maxVShift - minVShift;
  int charCellW = gridW;
  int charCellH = charH + vShiftRange;
  int charGlobalYShift = -minVShift;
  
  for (int k = 0; k < numChars; k++) {
    int rowNum = k / charsPerRow;
    int colNum = k % charsPerRow;
    int baseX = colNum * gridW;
    int baseY = rowNum * gridH;
    
    std::string sectionNumStr = asHex(k, 4);
    std::string sectionName = std::string("char") + sectionNumStr;
    
    TBitmapFontChar data;
    data.readFromIni(index, sectionName);
    data.grp.resize(gridW, gridH);
    data.grp.clearTransparent();
    data.grp.copy(sheet, TRect(0, 0, 0, 0), TRect(baseX, baseY, gridW, gridH));
      
    bool charHasDefinition = index.hasSection(sectionName);
    
    if ((data.glyphWidth == -1)) {
      // find rightmost opaque pixel
      int width = -1;
      for (int j = 0; j < gridH; j++) {
        for (int i = 0; i < gridW; i++) {
          TColor color = sheet.getPixel(baseX + i, baseY + j);
//          if ((color.a() == TColor::fullAlphaOpacity)
//              && ((color.r() != 0) || (color.g() != 0) || (color.b() != 0))) {
          if ((color.a() == TColor::fullAlphaOpacity)) {
            if (i > width) width = i;
          }
        }
      }
      
      if (width != -1) data.glyphWidth = width + 1 + autoWidthOffset;
      else data.glyphWidth = 0;
    }
    
    if ((data.advanceWidth == -1)) {
      data.advanceWidth = data.glyphWidth + 1;
    }
    
    fontChars.push_back(data);
  }
  
  kerningMatrix.resize(fontChars.size(), fontChars.size());
  
  if (noKerning) {
    for (int j = 0; j < fontChars.size(); j++) {
      for (int i = 0; i < fontChars.size(); i++) {
        kerningMatrix.data(i, j) = 0;
      }
    }
  }
  else {
    autoComputeKernings();
  }
}

void TBitmapFont:: autoComputeKernings() {
  for (int j = 0; j < fontChars.size(); j++) {
    for (int i = 0; i < fontChars.size(); i++) {
      int firstIndex = j;
      int secondIndex = i;
      
      TBitmapFontChar& first = fontChars[firstIndex];
      TBitmapFontChar& second = fontChars[secondIndex];
      
      int kerning;
      if (first.noKerningIfFirst) kerning = 0;
      else if (second.noKerningIfSecond) kerning = 0;
      else kerning = computeKerning(first, second, autoKerningMode);
      
      // HACK: this is too damn complicated.
      // don't double-kern unkerned characters with both left/right
      // offsets.
      if (first.noKerningIfFirst
          && second.noKerningIfSecond
          && ((first.extraKerningIfLeft != 0)
              && (second.extraKerningIfRight != 0))) {
        kerning += first.extraKerningIfLeft;
      }
      else {
        kerning += first.extraKerningIfLeft;
        kerning += second.extraKerningIfRight;
      }
      
//      if (!first.noKerningIfFirst && !second.noKerningIfSecond)
        kerning += autoKerningOffset;
      
      kerningMatrix.data(i, j) = kerning;
    }
  }
}

void TBitmapFont::render(TGraphic& dst, TStream& msg,
            const TThingyTable& table) const {
  std::vector<int> msgIds;
  
  TStream& ifs = msg;
  
  while (!ifs.eof()) {
    TThingyTable::MatchResult result = table.matchTableEntry(ifs);
    if (result.id == -1) {
      throw TGenericException(T_SRCANDLINE,
                              "TBitmapFont::render()",
                              std::string("At offset ")
                                + TStringConversion::intToString(
                                    ifs.tell(),
                                    TStringConversion::baseHex)
                                + ": unknown character '"
                                + TStringConversion::intToString(
                                    (unsigned char)ifs.peek(),
                                    TStringConversion::baseHex)
                                + "'");
    }
    
    int id = result.id;
    
    if ((id < 0) || (id > fontChars.size())) {
      throw TGenericException(T_SRCANDLINE,
                              "TBitmapFont::render()",
                              std::string("At offset ")
                                + TStringConversion::intToString(
                                    ifs.tell(),
                                    TStringConversion::baseHex)
                                + ": out-of-range character ID "
                                + TStringConversion::intToString(
                                    id,
                                    TStringConversion::baseHex));
    }
    
    // save new id
    msgIds.push_back(id);
  }
  
  renderPrecodedString(dst, msgIds);
}

void TBitmapFont::renderPrecodedString(TGraphic& dst,
    std::vector<int> msgIds) const {
  
  int w = 0;
  int h = charH + (maxVShift - minVShift);
  
  // look up characters and determine total message size
  int lastId = -1;
  int x = 0;
  for (unsigned int i = 0; i < msgIds.size(); i++) {
    int id = msgIds[i];
    
    if ((id < 0) || (id > fontChars.size())) {
      throw TGenericException(T_SRCANDLINE,
                              "TBitmapFont::renderPrecodedString()",
                              std::string("At char num ")
                                + TStringConversion::intToString(
                                    i,
                                    TStringConversion::baseHex)
                                + ": out-of-range character ID "
                                + TStringConversion::intToString(
                                    id,
                                    TStringConversion::baseHex));
    }
    
    // look up char params
    const TBitmapFontChar& fontChar = fontChars.at(id);
    
    // apply kerning with previous character
    int kerning = 0;
    if (lastId != -1) {
      kerning = getKerning(lastId, id);
    }
    x += kerning;
    
    // extend width
//    w = x + fontChar.glyphWidth;
    w = x + fontChar.advanceWidth;
    
    // update x from advance width
    x += fontChar.advanceWidth;
    
    // remember last ID
    lastId = id;
  }
  
  // resize graphic to output w/h
  dst.resize(w, h);
  dst.clearTransparent();
  
  // copy characters to graphic
  x = 0;
  lastId = -1;
  for (unsigned int i = 0; i < msgIds.size(); i++) {
    int id = msgIds[i];
    const TBitmapFontChar& fontChar = fontChars.at(id);
    
    // apply kerning with previous character
    int kerning = 0;
    if (lastId != -1) {
      kerning = getKerning(lastId, id);
    }
    x += kerning;
    
    // copy char graphic
    int y = -minVShift + fontChar.vShift;
    dst.blit(fontChar.grp,
             TRect(x, y, 0, 0));
    
    // apply advance width
    x += fontChar.advanceWidth;
    
    // remember last ID
    lastId = id;
  }
}

int TBitmapFont::numFontChars() const {
  return fontChars.size();
}

TBitmapFontChar& TBitmapFont::fontChar(int index) {
  return fontChars.at(index);
}

const TBitmapFontChar& TBitmapFont::fontChar(int index) const {
  return fontChars.at(index);
}

void TBitmapFont::exportKerningMatrix(TStream& ofs) const {
  for (int j = 0; j < kerningMatrix.h(); j++) {
    for (int i = 0; i < kerningMatrix.w(); i++) {
      ofs.writes8(kerningMatrix.data(i, j));
    }
  }
}

bool TBitmapFont::pixelIsNotBackground(const TGraphic& grp, int x, int y) {
  if (x < 0) return false;
  else if (x >= grp.w()) return false;
  else if (y < 0) return false;
  else if (y >= grp.h()) return false;

  TColor color = grp.getPixel(x, y);
//  if ((color.a() == TColor::fullAlphaOpacity)
//      && ((color.r() != 0) || (color.g() != 0) || (color.b() != 0))) {
  if ((color.a() == TColor::fullAlphaOpacity)) {
    return true;
  }
  else {
    // transparent
    return false;
  }
}

int TBitmapFont::computeKerning(
    const TBitmapFontChar& first, const TBitmapFontChar& second,
    AutoKerningMode autoKerningMode) {
  
  // FIXME: does not take vshift into account
  
  // do not apply kerning if first character is blank
  bool firstHasSolidPixel = false;
  for (int j = 0; j < first.grp.h(); j++) {
    for (int i = 0; i < first.grp.w(); i++) {
      if (pixelIsNotBackground(first.grp, i, j)) {
        firstHasSolidPixel = true;
        break;
      }
    }
  }
  if (!firstHasSolidPixel) return 0;

  int kerning = 0;
//  for (int i = first.advanceWidth - 1; i >= 1; i--) {
  bool done = false;
  bool secondHasSolidPixel = false;
//  int columnOfFirstSolidPixelInSecond = -1;
  for (int k = 1; k < first.advanceWidth; k++) {
    for (int i = 0; i < k; i++) {
      for (int j = 0; j < second.grp.h(); j++) {
//        int shift = -k;
        
        if (pixelIsNotBackground(second.grp, i, j)) {
//          if (!secondHasSolidPixel)
//            columnOfFirstSolidPixelInSecond = i;
          secondHasSolidPixel = true;
          
          int firstBaseX = first.advanceWidth - k + i;
          int firstBaseY = j;
//          std::cerr << "check: " << endl
//            << "  second: " << i << " " << j << std::endl
//            << "  first:  " << firstBaseX << " " << firstBaseY << endl;
          
/*          if (false   
//              || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY - 1)
              || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY - 1)
//              || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY - 1)
              || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY)
              || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY)
              || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY)
//              || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY + 1)
              || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY + 1)
//              || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY + 1)
             ) {
//            std::cerr << "done" << std::endl;
            done = true;
          } */
          
          if (autoKerningMode == autokern_cross) {
            if (false   
  //              || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY - 1)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY - 1)
  //              || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY - 1)
                || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY)
                || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY)
  //              || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY + 1)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY + 1)
  //              || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY + 1)
               ) {
  //            std::cerr << "done" << std::endl;
              done = true;
            }
          }
          else if (autoKerningMode == autokern_square) {
            if (false   
                || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY - 1)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY - 1)
                || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY - 1)
                || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY)
                || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY)
                || pixelIsNotBackground(first.grp, firstBaseX - 1, firstBaseY + 1)
                || pixelIsNotBackground(first.grp, firstBaseX, firstBaseY + 1)
                || pixelIsNotBackground(first.grp, firstBaseX + 1, firstBaseY + 1)
               ) {
              done = true;
            }
          }
          
        }
        
        if (done) break;
      }
      
      if (done) break;
    }
    
    if (done) break;
    else {
      --kerning;
//      std::cerr << "kerning changed to " << kerning << std::endl;
    }
  }
  
  // do not apply kerning if second character is blank
  if (!secondHasSolidPixel) return 0;
  
  // if we did not detect any collision even after moving the second
  // character all the way to the left... well, all we can do is
  // guess what will actually look good
//  if (!done) return (kerning >= 0) ? 0 : kerning + 1;
  if (!done) return 0;
  
  // if there are blank columns at the beginning of second,
  // assume they are intentional and offset the kerning by that many
  // columns in order to preserve them
/*  if (columnOfFirstSolidPixelInSecond != -1) {
//    std::cerr << columnOfFirstSolidPixelInSecond << std::endl;
    kerning += columnOfFirstSolidPixelInSecond;
  }*/
  
  return kerning;
}

int TBitmapFont::getKerning(int first, int second) const {
  return kerningMatrix.data(second, first);
}

void TBitmapFont::setKerning(int first, int second, int value) {
  kerningMatrix.data(second, first) = value;
}

void TBitmapFont::offsetKerning(int first, int second, int offset) {
  kerningMatrix.data(second, first) += offset;
}


}
