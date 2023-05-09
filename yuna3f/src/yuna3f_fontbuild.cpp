#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include "util/TOfstream.h"
#include "util/TIfstream.h"
#include "util/TStringConversion.h"
#include "util/TBitmapFont.h"
#include "util/TOpt.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;
using namespace BlackT;

static int charW = 12;
static int charH = 12;

static int kerningBaseOffset = 1;
const static int minRunLength = 3;

// colors at or below this level are ignored when computing kerning
//const static int colorDarknessThreshold = 2;
static int colorDarknessThreshold = 4;

//static int assumedOutlineThickness = 0;

void charToData(const TGraphic& src,
                int xOffset, int yOffset,
                TStream& ofs) {
  for (int j = 0; j < charH; j++) {
    
/*    for (int i = 0; i < charW / 2; i++) {
      int x = xOffset + (i * 2);
      int y = yOffset + j;
      TColor color1 = src.getPixel(x + 0, y);
      TColor color2 = src.getPixel(x + 1, y);
      
      int output = 0;
      if ((color1.a() != TColor::fullAlphaTransparency))
        output |= ((color1.r() / 16) << 0);
      if ((color2.a() != TColor::fullAlphaTransparency))
        output |= ((color2.r() / 16) << 4);
      
      ofs.writeu8(output);
    } */
    
    for (int i = 0; i < (charW + 3) / 4; i++) {
      int x = xOffset + (i * 4);
      int y = yOffset + j;
      TColor color1 = src.getPixel(x + 0, y);
      TColor color2 = src.getPixel(x + 1, y);
      TColor color3 = src.getPixel(x + 2, y);
      TColor color4 = src.getPixel(x + 3, y);
      
      int color1Val = color1.r() / 16;
      int color2Val = color2.r() / 16;
      int color3Val = color3.r() / 16;
      int color4Val = color4.r() / 16;
      
      // due to some clumsy programming on the original developers' part,
      // color 0xF cannot be used. this color is rendered as transparent
      // under certain circumstances, which would be fine except that
      // the game will sometimes draw the same string on top of itself with
      // different palettes to achieve the effect of highlighting options
      // in a menu. in this case, having transparency will wreck everything.
      // i didn't design it this way, so don't blame me!
      // the original font is like this too.
      if (color1Val == 0xF) color1Val = 0xE;
      if (color2Val == 0xF) color2Val = 0xE;
      if (color3Val == 0xF) color3Val = 0xE;
      if (color4Val == 0xF) color4Val = 0xE;
      
      int output = 0;
//      if ((color1.a() != TColor::fullAlphaTransparency))
      if ((color1.a() != TColor::fullAlphaTransparency)
          && (color1.r() != 0))
        output |= (color1Val << 12);
      if ((color2.a() != TColor::fullAlphaTransparency)
          && (color2.r() != 0))
        output |= (color2Val << 8);
      if ((color3.a() != TColor::fullAlphaTransparency)
          && (color3.r() != 0))
        output |= (color3Val << 4);
      if ((color4.a() != TColor::fullAlphaTransparency)
          && (color4.r() != 0))
        output |= (color4Val << 0);
      
      ofs.writeu16be(output);
    }
    
    // output a word of padding: this is used when compositing
    // a new character with existing content
//    ofs.writeu16le(0);
  }
}

void charToData8bpp(const TGraphic& src,
                int xOffset, int yOffset,
                TStream& ofs) {
  for (int j = 0; j < charH; j++) {
    for (int i = 0; i < (charW + 1) / 2; i++) {
      int x = xOffset + (i * 2);
      int y = yOffset + j;
      TColor color1 = src.getPixel(x + 0, y);
      TColor color2 = src.getPixel(x + 1, y);
      
      int color1Val = color1.r();
      int color2Val = color2.r();
      
      // due to some clumsy programming on the original developers' part,
      // color 0xF cannot be used. this color is rendered as transparent
      // under certain circumstances, which would be fine except that
      // the game will sometimes draw the same string on top of itself with
      // different palettes to achieve the effect of highlighting options
      // in a menu. in this case, having transparency will wreck everything.
      // i didn't design it this way, so don't blame me!
      // the original font is like this too.
//      if (color1Val == 0xFF) color1Val = 0xEE;
//      if (color2Val == 0xFF) color2Val = 0xEE;
      
      int output = 0;
//      if ((color1.a() != TColor::fullAlphaTransparency))
      if ((color1.a() != TColor::fullAlphaTransparency)
          && (color1.r() != 0))
        output |= (color1Val << 8);
      if ((color2.a() != TColor::fullAlphaTransparency)
          && (color2.r() != 0))
        output |= (color2Val << 0);
      
      ofs.writeu16be(output);
    }
    
    // output a word of padding: this is used when compositing
    // a new character with existing content
//    ofs.writeu16le(0);
  }
}

bool colorIsDarkEnough(TColor color) {
  int r = color.r() >> 4;
  int g = color.g() >> 4;
  int b = color.b() >> 4;
  
  if ((r <= colorDarknessThreshold)
      && (g <= colorDarknessThreshold)
      && (b <= colorDarknessThreshold))
    return true;
  
  return false;
}

void alterCharBitmap(TBitmapFontChar& c) {
  // strip dark colors
  for (int j = 0; j < c.grp.h(); j++) {
    for (int i = 0; i < c.grp.w(); i++) {
      TColor color = c.grp.getPixel(i, j);
      if (colorIsDarkEnough(color)) {
        c.grp.setPixel(i, j, TColor(color.r(), color.g(), color.b(),
          TColor::fullAlphaTransparency));
      }
    }
  }
  
/*  if (assumedOutlineThickness > 0) {
    // left shift
    for (int j = 0; j < c.grp.h(); j++) {
      for (int i = 0; i < c.grp.w() - assumedOutlineThickness; i++) {
        c.grp.getPixel(i, j) = c.grp.getPixel(i + assumedOutlineThickness, j);
      }
      
      for (int i = c.grp.w() - assumedOutlineThickness; i < c.grp.w(); i++) {
        c.grp.setPixel(i, j, TColor(0, 0, 0, TColor::fullAlphaTransparency));
      }
    }
  }*/
}

int getRunLength(const std::vector<int>& src, int baseIndex) {
  int length = 1;
  for (int i = baseIndex; i < src.size() - 1; i++) {
    if ((src[i] + 1) != src[i + 1]) break;
    ++length;
  }
  return length;
}

void makeKerningList(TBitmapFont& font,
    TStream& indexOfs, TStream& dataOfs) {
  // converts the kerning data to a sparse matrix format.
  // format of output is as follows:
  // - index block, consisting of (numChars) little-endian halfwords.
  //   if an entry is -1 (0xFFFF), the character is not kerned.
  //   otherwise, it is an offset from the beginning of the data block
  //   giving the start position of the kerning entries for a character.
  // - data block, consisting of a series of sequential entries.
  //   at each entry's start, assume kerning to be -1.
  //   each following byte is the ID of a character for which this character,
  //   if preceded by the IDed character, should be kerned by -1.
  //   the following special values control processing:
  //   - 0xFX = add this value (signed byte) to the kerning.
  //            the entries that follow now use this new kerning.
  //   - 0xF0 = end of entry; stop processing
  //   - 0xEX = sequential incremental run:
  //            repeat the following byte (X+2) times, incrementing each time.
  //            e.g. "E2 01" means "01 02 03 04 05"
  //   if end of entry is reached without finding the target character,
  //   assume a kerning of 0.
  
//  TBufStream kerningIfs;
//  font.exportKerningMatrix(kerningIfs);
  for (int j = 0; j < font.numFontChars(); j++) {
    std::map<int, std::vector<int> > kerningToPrevIdList;
    bool hasNonzeroEntry = false;
    for (int i = 0; i < font.numFontChars(); i++) {
      int second = j;
      int first = i;
      
      int kerning = font.getKerning(first, second);
      if (kerning != 0) hasNonzeroEntry = true;
      
//      if (first == 0x59 && second == 0x63)
//        std::cerr << "test kerning: " << kerning << std::endl;
      
      // HACK: the way we're processing the characters means that
      // any character that doesn't have kerning totally disabled
      // will have a kerning of -1 or lower, with -1 the most common
      // value.
      // to make use of this, the numbers we input are offset by 1.
      // the expectation is that the code that uses this table will
      // have its own specific blacklist for the non-kerned characters
      // so that it ignores them, and for anything else, it will
      // subtract 1 from the returned kerning value.
      // (note: blacklist can probably be implemented by checking whether
      // previous character has a 0xFFFF entry in the index)
//      kerning += kerningBaseOffset;
      kerningToPrevIdList[kerning].push_back(first);
    }
    
    // if all entries zero, do not generate a data entry
    // and flag index entry as nonexistent
    if (!hasNonzeroEntry) {
      // mark as empty
      indexOfs.writeu16le(0xFFFF);
      continue;
    }
    
    indexOfs.writeu16le(dataOfs.tell());
    
//    int lastNonemptyKerning = -1;
    int lastNonemptyKerning = -kerningBaseOffset - 1;
    for (int i = -kerningBaseOffset - 1; i >= -(kerningBaseOffset + 16); --i) {
      auto findIt = kerningToPrevIdList.find(i);
      if (findIt == kerningToPrevIdList.end()) {
        continue;
      }
      
      int gap = lastNonemptyKerning - i;
      lastNonemptyKerning = i;
      // add "next kerning" commands until current target reached
//      for (int k = 0; k < gap; k++) {
//        dataOfs.writeu8(0xFE);
//      }
      if (gap > 0) {
        dataOfs.writes8(-gap);
      }
      
      std::vector<int>& prevIdList = findIt->second;
//      for (auto value: prevIdList) {
//        dataOfs.writeu8(value);
//      }
      
      for (int k = 0; k < prevIdList.size(); ) {
        int runLen = getRunLength(prevIdList, k);
        
        if (runLen >= minRunLength) {
          if (runLen >= (0xF + minRunLength)) runLen = (0xF + minRunLength);
          dataOfs.writeu8(0xE0 | (runLen - minRunLength));
          dataOfs.writeu8(prevIdList[k]);
          k += runLen;
        }
        else {
          dataOfs.writeu8(prevIdList[k]);
          ++k;
        }
      }
    }
    
    // write end-of-entry marker
    dataOfs.writeu8(0xF0);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yuna 3 FE font builder" << endl;
//    cout << "Usage: " << argv[0] << " <font>"
//      << " <outfont> <outwidthfile> <outkerningindex> <outkerningdata>"
//      << " [outglyphwfile] [outkerningmatrix]"
//      << endl;
    cout << "Usage: " << argv[0] << " <font> <outfont> [options]"
      << endl;
    cout << "Options:" << endl;
    cout << "  -w    Sets output character width"
      << " (default: " << charW << ")" << endl;
    cout << "  -h    Sets output character height"
      << " (default: " << charH << ")" << endl;
    cout << "  -k    Sets base kerning offset"
      << " (default: " << kerningBaseOffset << ")" << endl;
    cout << "  -d    Sets kerning darkenss threshold"
      << " (default: " << colorDarknessThreshold << ")" << endl;
    cout << "  --8bpp    Output 8bpp instead of 4bpp" << endl;
    cout << "  --offsetkern [second] [offset]" << endl
      << "           Adds OFFSET to all nonzero kerning pairs in which" << endl
      << "           SECOND is the second character," << endl
      << "           and the first character is not flagged as nonkernable" << endl
      << "           if it is the first character in a pair" << endl;
    cout << "  --offsetkernpair [first] [second] [offset]" << endl
      << "           Adds OFFSET to a kerning pair" << endl;
    cout << "  --globalzkern [offset]" << endl
      << "           Adds OFFSET to all zero kerning pairs" << endl
      << "           in which neither character is flagged as nonkernable" << endl;
    cout << "  --globalnkkern [offset]" << endl
      << "           Adds OFFSET to all kerning pairs in which either character"
      << "           is flagged as nonkernable" << endl;
//    cout << "  --outlinewidth    Sets width of existing font outline" << endl;
    
    return 0;
  }
  
/*  string fontName(argv[1]);
//  string outFontFileName(argv[2]);
//  int maxChars = TStringConversion::stringToInt(string(argv[2]));
  string outFontFileName(argv[2]);
  string outWidthFileName(argv[3]);
  string outKerningIndexFileName(argv[4]);
  string outKerningDataFileName(argv[5]);
  string outGlyphWidthFileName;
  if (argc >= 7) outGlyphWidthFileName = string(argv[6]);
  string outKerningMatrixFileName;
  if (argc >= 8) outKerningMatrixFileName = string(argv[7]);*/
  
  string fontName(argv[1]);
//  string outFontFileName(argv[2]);
//  int maxChars = TStringConversion::stringToInt(string(argv[2]));
  string outFontPrefix(argv[2]);
  
  string outFontFileName = outFontPrefix + "font.bin";
  string outWidthFileName = outFontPrefix + "width.bin";
  string outKerningIndexFileName = outFontPrefix + "kerning_index.bin";
  string outKerningDataFileName = outFontPrefix + "kerning_data.bin";
  string outGlyphWidthFileName = outFontPrefix + "glyph_width.bin";
  string outKerningMatrixFileName = outFontPrefix + "kerning_matrix.bin";
  
  TOpt::readNumericOpt(argc, argv, "-w", &charW);
  TOpt::readNumericOpt(argc, argv, "-h", &charH);
  TOpt::readNumericOpt(argc, argv, "-k", &kerningBaseOffset);
  TOpt::readNumericOpt(argc, argv, "-d", &colorDarknessThreshold);
//  TOpt::readNumericOpt(argc, argv, "--outlinewidth",
//    &assumedOutlineThickness);
  bool mode8bpp = false;
  if (TOpt::hasFlag(argc, argv, "--8bpp"))
    mode8bpp = true;
  
  TBitmapFont font;
  font.load(fontName);
  
  TBufStream fontofs;
  TBufStream widthofs;
  
//  if (font.numFontChars() < maxChars) maxChars = font.numFontChars();
  
//  for (int i = 0; i < maxChars; i++) {
  for (int i = 0; i < font.numFontChars(); i++) {
    const TBitmapFontChar& fontChar = font.fontChar(i);
    
    int width = fontChar.advanceWidth;
//    fontofs.writeu32le(width);
    widthofs.writeu8(width);
    if (mode8bpp)
      charToData8bpp(fontChar.grp, 0, 0, fontofs);
    else
      charToData(fontChar.grp, 0, 0, fontofs);
  }
  
  fontofs.save(outFontFileName.c_str());
  widthofs.save(outWidthFileName.c_str());
  
//  TBitmapFont fontOrig = font;
  
  // prepare alt version of the font used to compute kerning
//  TBitmapFont fontAlt = font;
  for (int i = 0; i < font.numFontChars(); i++) {
    TBitmapFontChar& c = font.fontChar(i);
    alterCharBitmap(c);
  }
  font.autoComputeKernings();
  
  for (int i = 0; i < argc - 1; i++) {
    if (std::string(argv[i]).compare("--globalnkkern") == 0) {
      int offset = TStringConversion::stringToInt(std::string(argv[i + 1]));
      
      for (int i = 0; i < font.numFontChars(); i++) {
        for (int j = 0; j < font.numFontChars(); j++) {
          int first = i;
          int second = j;
          if ((font.fontChar(first).noKerningIfFirst == true)
              || (font.fontChar(second).noKerningIfSecond == true)) {
            font.offsetKerning(first, second, offset);
          }
        }
      }
    }
  }
  
  for (int i = 0; i < argc - 1; i++) {
    if (std::string(argv[i]).compare("--globalzkern") == 0) {
      int offset = TStringConversion::stringToInt(std::string(argv[i + 1]));
      
      for (int i = 0; i < font.numFontChars(); i++) {
        for (int j = 0; j < font.numFontChars(); j++) {
          int first = i;
          int second = j;
          if ((font.getKerning(first, second) == 0)
              && !(font.fontChar(first).noKerningIfFirst == true)
              && !(font.fontChar(second).noKerningIfSecond == true)) {
            font.offsetKerning(first, second, offset);
          }
        }
      }
    }
  }
  
  for (int i = 0; i < argc - 2; i++) {
    if (std::string(argv[i]).compare("--offsetkern") == 0) {
      int second = TStringConversion::stringToInt(std::string(argv[i + 1]));
      int offset = TStringConversion::stringToInt(std::string(argv[i + 2]));
      
      for (int i = 0; i < font.numFontChars(); i++) {
        if (font.getKerning(i, second) != 0
            && !(font.fontChar(i).noKerningIfFirst == true)) {
          font.offsetKerning(i, second, offset);
        }
      }
    }
  }
  
  for (int i = 0; i < argc - 3; i++) {
    if (std::string(argv[i]).compare("--offsetkernpair") == 0) {
      int first = TStringConversion::stringToInt(std::string(argv[i + 1]));
      int second = TStringConversion::stringToInt(std::string(argv[i + 2]));
      int offset = TStringConversion::stringToInt(std::string(argv[i + 3]));
      
//      std::cerr << "old: " << first << " " << second << " " << font.getKerning(first, second) << std::endl;
      font.offsetKerning(first, second, offset);
//      std::cerr << "new: " << first << " " << second << " " << font.getKerning(first, second) << std::endl;
    }
  }
  
//  for (int i = 0; i < font.numFontChars(); i++) {
//    std::cerr << std::hex << i << ": " << std::dec << font.getKerning(i, 0x59) << std::endl;
//  }
  
  // for any columns on the left side of a character that are fully transparent
  // in the ORIGINAL, UNALTERED font, apply an extra offset to kerning pairs
  // for which that character is the second of the pair, so the empty columns
  // are preserved
/*  for (int k = 0; k < font.numFontChars(); k++) {
    TBitmapFontChar& c = fontOrig.fontChar(k);
    int firstNontransparentColumn = -1;
    for (int j = 0; j < c.grp.h(); j++) {
      for (int i = 0; i < c.grp.w(); i++) {
        if (c.grp.getPixel(i, j).a() != TColor::fullAlphaTransparency) {
          firstNontransparentColumn = i;
          break;
        }
      }
      
      if (firstNontransparentColumn >= 0) break;
    }
    
    if (firstNontransparentColumn > 0) {
      for (int i = 0; i < font.numFontChars(); i++) {
        int origKerning = font.getKerning(i, k);
        if (origKerning != 0)
          font.setKerning(i, k, origKerning + firstNontransparentColumn);
      }
    }
  }*/
  
//  std::cerr << fontName << std::endl;
//  std::cerr << "test kerning: " << font.getKerning((int)'i' - 0x10, (int)'s' - 0x10) << std::endl;
  
//  TBufStream kerningOfs;
//  font.exportKerningMatrix(kerningOfs);
//  kerningOfs.save("test_kern.bin");
  
  TBufStream kerningIndexOfs;
  TBufStream kerningDataOfs;
  makeKerningList(font, kerningIndexOfs, kerningDataOfs);
  
  kerningIndexOfs.save(outKerningIndexFileName.c_str());
  kerningDataOfs.save(outKerningDataFileName.c_str());
  
  if (!outGlyphWidthFileName.empty()) {
    TBufStream ofs;
    
    for (int i = 0; i < font.numFontChars(); i++) {
      TBitmapFontChar& fontChar = font.fontChar(i);
      ofs.writeu8(fontChar.glyphWidth);
    }
    
    ofs.save(outGlyphWidthFileName.c_str());
  }
  
  if (!outKerningMatrixFileName.empty()) {
    TBufStream ofs;
    font.exportKerningMatrix(ofs);
    ofs.save(outKerningMatrixFileName.c_str());
  }
  
  return 0;
}
