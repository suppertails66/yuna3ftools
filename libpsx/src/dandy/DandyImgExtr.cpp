#include "dandy/DandyImgExtr.h"
#include "psx/PsxPalette.h"
#include "psx/PsxBitmap.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TGraphic.h"
#include "util/TColor.h"
#include "util/TPngConversion.h"
#include <map>

using namespace BlackT;

namespace Psx {


const double DandyImgExtr::colorScale = (double)255 / (double)31;

/*void DandyImgExtr::extract(BlackT::TStream& ifs, std::string outname) {
  TGraphic grp;
  getGraphic(ifs, grp);
  TPngConversion::graphicToRGBAPng(outname, grp);
}

void DandyImgExtr::getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp) {
  // header
  
  // usually/always 0x10.
  // any significance to this?
  // header size?
  int unk1 = ifs.readu32le();
  // &0x01 = 0 if 4-bit (or 2-bit????), 1 if 8-bit?
  // &0x02 = unknown
  //         set in SINKOU.PAC-0 (a large "image" containing
  //         nearly all 2D resources for the title sequence,
  //         in multiple formats, as a single 1024x512 bitmap)
  // &0x04 = ?
  // &0x08 = 1 if has internal palette?
  //         if set, header has two extra fields?
  //         if first field is zero, 2-bit mode????
  // &0x10 = ?
  // &0x20 = ?
  // &0x40 = ?
  // &0x80 = ?
  int format = ifs.readu32le();
  // not correct?
  // two 16-bit fields?
  // ADV-1-0 sub1 has 00 00 01 00...
  int subdataSize = ifs.readu32le();
  
  // font files, at the least, use a 2-bit mode
  // which is stored as if it were 4-bit, and has
  // 16 colors in the palette, but only the first 4
  // are used and pixel data is in fact 2-bit.
  bool mode2Bit = false;
  
  // palette info chunk?
  if ((format & 0x08) != 0) {
    // ???
    // often zero?
    int unk2 = ifs.readu32le();
    // ???
    // e.g. 10 00 01 00 in font graphics (2-bit masquerading as 4-bit)
    //      00 01 01 00 in character portraits (8-bit)
    // first halfword = number of colors?
    // second halfword = ???
    //                   complete guess: number of halfwords per color??????
    int unk3 = ifs.readu32le();
  
    // ?????????
    // this is probably not correct, but there's no obvious
    // flag for it...
    // is it hardcoded where needed?
    if (unk2 == 0) mode2Bit = true;
  }
  
  std::map<int, TColor> colorTable;
  
  int numColors = 0;
  if ((format & 0x08) != 0) {
    if ((format & 0x01) == 0) {
      // 2-bit?
      // but always has 16 colors.
      // is there a separate flag somewhere indicating 2-bit mode?
      numColors = 16;
    }
    else {
      // 8-bit
      numColors = 256;
    }
    
//    else {
//      throw TGenericException(T_SRCANDLINE,
//                              "DandyImgExtr::extract()",
//                              std::string("Unknown image format ID: ")
//                                + TStringConversion::intToString(format));
//    }
  }
  else {
    // 8-bit, no palette?
    numColors = 0;
    
    // grayscale
    if ((format & 0x01) == 0) {
      // 2-bit
      for (int i = 0; i < 16; i++) {
        int val = i | (i << 4);
        colorTable[i] = TColor(val, val, val);
      }
    }
    else {
      // 8-bit
      for (int i = 0; i < 256; i++) {
        colorTable[i] = TColor(i, i, i);
      }
    }
  }
  
  // palette
  if ((format & 0x08) != 0) {
    for (int i = 0; i < numColors; i++) {
      int rawColor = ifs.readu16le();
      
      int r = ((rawColor & (0x1F << 0)) >> 0) * colorScale;
      int g = ((rawColor & (0x1F << 5)) >> 5) * colorScale;
      int b = ((rawColor & (0x1F << 10)) >> 10) * colorScale;
      
      TColor color(r, g, b);
      colorTable[i] = color;
    }
    
    // size of remaining data (inclusive)?
    // why is this part of the palette chunk?
    int subunk1 = ifs.readu32le();
  }
  
  // subheader
  // this field is probably the GPU destination "address"
  // (or zero if location is not fixed/not loaded directly to GPU).
  // top halfword = line number (0x800-byte "page")?
  // low halfword = offset within line in halfwords? (e.g. 0x200 = 0x400 bytes)
  int subunk2 = ifs.readu32le();
  // width, but must be adjusted based on bit depth.
  // multiply by 2 for 8-bit, 4 for 4-bit to get true value.
  // (number of halfwords per line?
  // i think this is technically the "pitch" or something)
  int width = ifs.readu16le() * 2;
  // height is pixel-accurate
  int height = ifs.readu16le();
  
  // ?????????
  // this is probably not correct, but there's no obvious
  // flag for it...
  // is it hardcoded where needed?
//  if (subunk2 == 0) mode2Bit = true;
  
  if ((format & 0x1) == 0x00) {
    width *= 2;
    if (mode2Bit) width *= 2;
  }
  
  grp = TGraphic(width, height);
  grp.clearTransparent();
  
  if ((format & 0x1) == 0) {
    if (mode2Bit) {
      // 2-bit?
      for (int j = 0; j < height; j++) {
  //      for (int i = 0; i < width; i += 2) {
        for (int i = 0; i < width; i += 4) {
          BlackT::TByte next = ifs.readu8();
  //        grp.setPixel(i, j, colorTable[next & 0x0F]);
  //        grp.setPixel(i + 1, j, colorTable[(next & 0xF0) >> 4]);
          grp.setPixel(i, j, colorTable[next & 0x03]);
          grp.setPixel(i + 1, j, colorTable[(next & (0x03 << 2)) >> 2]);
          grp.setPixel(i + 2, j, colorTable[(next & (0x03 << 4)) >> 4]);
          grp.setPixel(i + 3, j, colorTable[(next & (0x03 << 6)) >> 6]);
        }
      }
    }
    else {
      for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i += 2) {
          BlackT::TByte next = ifs.readu8();
          grp.setPixel(i, j, colorTable[next & 0x0F]);
          grp.setPixel(i + 1, j, colorTable[(next & 0xF0) >> 4]);
        }
      }
    }
  }
  else {
    // 8-bit
    for (int j = 0; j < height; j++) {
      for (int i = 0; i < width; i++) {
        grp.setPixel(i, j, colorTable[ifs.readu8()]);
      }
    }
  }
} */

void DandyImgExtr::extract(BlackT::TStream& ifs, std::string outname) {
  TGraphic grp;
  
  getGraphic(ifs, grp);
  
  TPngConversion::graphicToRGBAPng(outname, grp);
}

void DandyImgExtr::getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp) {
  // header
  
  // usually/always 0x10.
  // any significance to this?
  // header size?
  int unk1 = ifs.readu32le();
  // &0x01 = 0 if 4-bit (or 2-bit????), 1 if 8-bit?
  // &0x02 = unknown
  //         set in SINKOU.PAC-0 (a large "image" containing
  //         nearly all 2D resources for the title sequence,
  //         in multiple formats, as a single 1024x512 bitmap)
  // &0x04 = ?
  // &0x08 = 1 if has internal palette?
  //         if set, header has two extra fields?
  //         if first field is zero, 2-bit mode????
  // &0x10 = ?
  // &0x20 = ?
  // &0x40 = ?
  // &0x80 = ?
  int format = ifs.readu32le();
  // not correct?
  // two 16-bit fields?
  // ADV-1-0 sub1 has 00 00 01 00...
  int subdataSize = ifs.readu32le();
  
  bool hasPalette = (format & 0x08) != 0;
  bool highColorMode = (format & 0x01) != 0;
  
  // used for mission graphics.
  // how is this detected?
  // hardcoded where needed?
  bool mode2Bit = false;
  
  if (hasPalette) {
    // ???
    // often zero?
    int unk2 = ifs.readu32le();
    // ???
    // e.g. 10 00 01 00 in font graphics (4-bit)
    //      00 01 01 00 in character portraits (8-bit)
    // first halfword = number of colors?
    // second halfword = ???
    //                   complete guess: number of halfwords per color??????
    int unk3 = ifs.readu32le();
  
    // ?????????
    // this is probably not correct, but there's no obvious
    // flag for it...
    // is it hardcoded where needed?
    if (unk2 == 0) mode2Bit = true;
  }
  
  int numColors = 0;
  if (hasPalette != 0) {
    if (!highColorMode) {
      // always use 16-color palette, even in 2-bit mode
      numColors = 16;
    }
    else {
      // 8-bit
      numColors = 256;
    }
  }
  
  // palette
  PsxPalette palette;
  if (hasPalette) {
    for (int i = 0; i < numColors; i++) {
      PsxColor color;
      color.read(ifs);
      
      color.setSemiTransparent(false);
      
      palette.setColor(i, color);
    }
    
    // size of pixel data in bytes (inclusive)?
    // why is this part of the palette chunk?
    int subunk1 = ifs.readu32le();
  }
  
  // subheader
  // this field is probably the GPU destination "address"
  // (or zero if location is not fixed/not loaded directly to GPU).
  // top halfword = line number (0x800-byte "page")?
  // low halfword = offset within line in halfwords? (e.g. 0x200 = 0x400 bytes)
  int subunk2 = ifs.readu32le();
  // width, but must be adjusted based on bit depth.
  // multiply by 2 for 8-bit, 4 for 4-bit to get true value.
  // (number of halfwords per line?
  // i think this is technically the "pitch" or something)
  int width = ifs.readu16le() * 2;
  // height is pixel-accurate
  int height = ifs.readu16le();
  
  // ?????????
  // this is probably not correct, but there's no obvious
  // flag for it...
  // is it hardcoded where needed?
//  if (subunk2 == 0) mode2Bit = true;
  
  if (!highColorMode) {
    width *= 2;
    if (mode2Bit) width *= 2;
  }
  
  PsxBitmap bitmap;
  bitmap.setPalette(palette);
  
  // read pixel data according to format
  if (!highColorMode) {
    if (mode2Bit) {
      bitmap.readPixelData(ifs, PsxBitmap::format_2bit, width, height);
    }
    else {
      bitmap.readPixelData(ifs, PsxBitmap::format_4bit, width, height);
    }
  }
  else {
    bitmap.readPixelData(ifs, PsxBitmap::format_8bit, width, height);
  }
  
  // export in color or grayscale as possible
  if (hasPalette) {
    bitmap.toGraphic(grp, false);
  }
  else {
    bitmap.toGrayscaleGraphic(grp);
  }
}

}
