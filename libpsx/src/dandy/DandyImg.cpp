#include "dandy/DandyImg.h"
#include "psx/PsxPalette.h"
#include "psx/PsxBitmap.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TGraphic.h"
#include "util/TColor.h"
#include "util/TPngConversion.h"
#include "util/TIniFile.h"
#include <map>

using namespace BlackT;

namespace Psx {


#define DANDYIMG_SAVE_INT_MACRO(INI, NAME) \
  INI.setValue("Settings",\
               #NAME,\
               TStringConversion::intToString(NAME))

#define DANDYIMG_LOAD_INT_MACRO(INI, NAME) \
  NAME = TStringConversion::stringToInt(INI.valueOfKey("Settings",\
               #NAME))


DandyImg::DandyImg()
  : headerUnk(0),
    headerUnk2(0),
    headerUnk3(0),
    format(format_4bit),
    formatFlag1(false),
    hasPalette(false),
    paletteUnk0(0),
    paletteUnk2(0),
    paletteUnk3(0),
    gpuDstLo(0),
    gpuDstHi(1),
    transparency(false),
    grayscale(false),
    semiTransparency(false)
    { }

void DandyImg::setTransparency(bool transparency__) {
  transparency = transparency__;
}

void DandyImg::setSemiTransparency(bool semiTransparency__) {
  semiTransparency = semiTransparency__;
}

void DandyImg::setGrayscale(bool grayscale__) {
  grayscale = grayscale__;
}

void DandyImg::setForcedPalette(const PsxPalette& palette) {
  bitmap.setPalette(palette);
  grayscale = false;
  hasPalette = true;
}

void DandyImg::stripPalette() {
  hasPalette = false;
  bitmap.setPalette(PsxPalette());
}

void DandyImg::read(TStream& ifs) {
  headerUnk = ifs.readu32le();
  
  int formatFlags = ifs.readu32le();
  
  headerUnk2 = ifs.readu16le();
  headerUnk3 = ifs.readu16le();
  
  // bit 0 = unset if low-color mode (2- or 4-bit), set if 8-bit color
  if ((formatFlags & 0x01) != 0) format = format_8bit;
  else format = format_4bit;
  
  // ?
  formatFlag1 = ((formatFlags & 0x02) != 0);
  
  hasPalette = ((formatFlags & 0x08) != 0);
  
  if (hasPalette) {
    // ???
    // often zero?
    paletteUnk0 = ifs.readu32le();
    // ???
    // e.g. 10 00 01 00 in font graphics (4-bit)
    //      00 01 01 00 in character portraits (8-bit)
    // first halfword = number of colors?
    // second halfword = ???
    //                   complete guess: number of halfwords per color??????
    paletteUnk2 = ifs.readu16le();
    paletteUnk3 = ifs.readu16le();
  
    // ?????????
    // this is probably not correct, but there's no obvious
    // flag for it...
    // is it hardcoded where needed?
    if ((format == format_4bit)
        && paletteUnk0 == 0) format = format_2bit;
  }
  
  int numColors = 0;
  if (hasPalette != 0) {
    if (format == format_8bit) {
      // 8-bit
      numColors = 256;
    }
    else {
      // always use 16-color palette, even in 2-bit mode
      numColors = 16;
    }
  }
  
  // palette
  PsxPalette palette;
  if (hasPalette) {
    for (int i = 0; i < numColors; i++) {
      PsxColor color;
      color.read(ifs);
      
//      color.setSemiTransparent(false);
      
      palette.setColor(i, color);
    }
    
    // size of pixel data in bytes (inclusive)?
    // why is this part of the palette chunk?
    int subunk1 = ifs.readu32le();
  }
  bitmap.setPalette(palette);
  
  // subheader
  // this field is probably the GPU destination "address"
  // (or zero if location is not fixed/not loaded directly to GPU).
  // top halfword = line number (0x800-byte "page")?
  // low halfword = offset within line in halfwords? (e.g. 0x200 = 0x400 bytes)
  gpuDstLo = ifs.readu16le();
  gpuDstHi = ifs.readu16le();
  // width, but must be adjusted based on bit depth.
  // multiply by 2 for 8-bit, 4 for 4-bit to get true value.
  // (number of halfwords per line?
  // i think this is technically the "pitch" or something)
  int pitch = ifs.readu16le();
  // height is pixel-accurate
  int height = ifs.readu16le();
  
//  std::cerr << std::hex << ifs.tell() << std::endl;
  
  int width = pitch;
  switch (format) {
  case format_2bit:
    width *= 8;
    break;
  case format_4bit:
    width *= 4;
    break;
  case format_8bit:
    width *= 2;
    break;
  default:
    break;
  }
  
  // read bitmap
  
  bitmap.setPalette(palette);
  
  // read pixel data according to format
  switch (format) {
  case format_2bit:
    bitmap.readPixelData(ifs, PsxBitmap::format_2bit, width, height);
    break;
  case format_4bit:
    bitmap.readPixelData(ifs, PsxBitmap::format_4bit, width, height);
    break;
  case format_8bit:
    bitmap.readPixelData(ifs, PsxBitmap::format_8bit, width, height);
    break;
  default:
    break;
  }
}

void DandyImg::save(std::string outprefix) const {
  
  // metadata
  
  {
    TIniFile ini;
//    ini.setValue("Settings",
//                 "headerUnk",
//                 TStringConversion::intToString(headerUnk));
    DANDYIMG_SAVE_INT_MACRO(ini, headerUnk);
    DANDYIMG_SAVE_INT_MACRO(ini, headerUnk2);
    DANDYIMG_SAVE_INT_MACRO(ini, headerUnk3);
    DANDYIMG_SAVE_INT_MACRO(ini, format);
    DANDYIMG_SAVE_INT_MACRO(ini, formatFlag1);
    DANDYIMG_SAVE_INT_MACRO(ini, hasPalette);
    DANDYIMG_SAVE_INT_MACRO(ini, paletteUnk0);
    DANDYIMG_SAVE_INT_MACRO(ini, paletteUnk2);
    DANDYIMG_SAVE_INT_MACRO(ini, paletteUnk3);
    DANDYIMG_SAVE_INT_MACRO(ini, gpuDstLo);
    DANDYIMG_SAVE_INT_MACRO(ini, gpuDstHi);
    DANDYIMG_SAVE_INT_MACRO(ini, transparency);
    DANDYIMG_SAVE_INT_MACRO(ini, semiTransparency);
    DANDYIMG_SAVE_INT_MACRO(ini, grayscale);
    
    ini.writeFile(outprefix + "-data.txt");
  }
  
  // palette
  
  if (hasPalette) {
    TBufStream ofs;
    
    switch (format) {
    case format_2bit:
    case format_4bit:
      bitmap.palette().write(ofs, 16);
      break;
    case format_8bit:
      bitmap.palette().write(ofs, 256);
      break;
    default:
      break;
    }
    
    ofs.save((outprefix + "-pal.bin").c_str());
  }

  // image data
  
  {
    TGraphic grp;
    
    if (!hasPalette || grayscale) {
      bitmap.toGrayscaleGraphic(grp);
    }
    else {
      bitmap.toGraphic(grp, transparency, semiTransparency);
    }
    
    TPngConversion::graphicToRGBAPng(outprefix + "-img.png", grp);
  }
}

void DandyImg::load(std::string inprefix) {
  
  // metadata
  
  {
    TIniFile ini;
    ini.readFile(inprefix + "-data.txt");
    
    DANDYIMG_LOAD_INT_MACRO(ini, headerUnk);
    DANDYIMG_LOAD_INT_MACRO(ini, headerUnk2);
    DANDYIMG_LOAD_INT_MACRO(ini, headerUnk3);
//    DANDYIMG_LOAD_INT_MACRO(ini, format);
    format = static_cast<Format>(
      TStringConversion::stringToInt(ini.valueOfKey("Settings", "format"))
      );
    DANDYIMG_LOAD_INT_MACRO(ini, formatFlag1);
    DANDYIMG_LOAD_INT_MACRO(ini, hasPalette);
    DANDYIMG_LOAD_INT_MACRO(ini, paletteUnk0);
    DANDYIMG_LOAD_INT_MACRO(ini, paletteUnk2);
    DANDYIMG_LOAD_INT_MACRO(ini, paletteUnk3);
    DANDYIMG_LOAD_INT_MACRO(ini, gpuDstLo);
    DANDYIMG_LOAD_INT_MACRO(ini, gpuDstHi);
    DANDYIMG_LOAD_INT_MACRO(ini, transparency);
    DANDYIMG_LOAD_INT_MACRO(ini, semiTransparency);
    DANDYIMG_LOAD_INT_MACRO(ini, grayscale);
  }
  
  // palette
  
  if (hasPalette) {
    TBufStream ifs;
    ifs.open((inprefix + "-pal.bin").c_str());
    
    PsxPalette palette;
    switch (format) {
    case format_2bit:
    case format_4bit:
      palette.read(ifs, 16);
      break;
    case format_8bit:
      palette.read(ifs, 256);
      break;
    default:
      break;
    }
    
    bitmap.setPalette(palette);
  }

  // image data
  
  {
    TGraphic grp;
    TPngConversion::RGBAPngToGraphic(inprefix + "-img.png", grp);
    
    switch (format) {
    case format_2bit:
      bitmap.setFormat(PsxBitmap::format_2bit);
      break;
    case format_4bit:
      bitmap.setFormat(PsxBitmap::format_4bit);
      break;
    case format_8bit:
      bitmap.setFormat(PsxBitmap::format_8bit);
      break;
    default:
      break;
    }
    
    if (!hasPalette || grayscale) {
      bitmap.fromGrayscaleGraphic(grp);
    }
    else {
      bitmap.fromGraphic(grp, transparency);
    }
  }
}

void DandyImg::write(BlackT::TStream& ofs) const {
  ofs.writeu32le(headerUnk);
  
  int formatFlags = 0;
  if (format == format_8bit) formatFlags |= 0x01;
  if (formatFlag1) formatFlags |= 0x02;
  if (hasPalette) formatFlags |= 0x08;
  
  ofs.writeu32le(formatFlags);
  
  ofs.writeu16le(headerUnk2);
  ofs.writeu16le(headerUnk3);
  
  if (hasPalette) {
    ofs.writeu32le(paletteUnk0);
    ofs.writeu16le(paletteUnk2);
    ofs.writeu16le(paletteUnk3);
  }
  
  int numColors = 0;
  if (hasPalette != 0) {
    if (format == format_8bit) {
      // 8-bit
      numColors = 256;
    }
    else {
      // always use 16-color palette, even in 2-bit mode
      numColors = 16;
    }
  }
  
  // palette
  int paletteRemainingFieldStart = 0;
  if (hasPalette) {
    bitmap.palette().write(ofs, numColors);
    
    paletteRemainingFieldStart = ofs.tell();
    // reserve space for remaining data count
    ofs.writeu32le(0);
  }
  
  // subheader
  // this field is probably the GPU destination "address"
  // (or zero if location is not fixed/not loaded directly to GPU).
  // top halfword = line number (0x800-byte "page")?
  // low halfword = offset within line in halfwords? (e.g. 0x200 = 0x400 bytes)
  ofs.writeu16le(gpuDstLo);
  ofs.writeu16le(gpuDstHi);
  
  int width = bitmap.w();
  int height = bitmap.h();
  
  int pitch = width;
  switch (format) {
  case format_2bit:
    pitch /= 8;
    break;
  case format_4bit:
    pitch /= 4;
    break;
  case format_8bit:
    pitch /= 2;
    break;
  default:
    break;
  }
  
  ofs.writeu16le(pitch);
  ofs.writeu16le(height);
  
  bitmap.writePixelData(ofs);
  
  // update palette remaining size field
  if (hasPalette) {
    int pos = ofs.tell();
    ofs.seek(paletteRemainingFieldStart);
    ofs.writeu32le(pos - paletteRemainingFieldStart);
    ofs.seek(pos);
  }
}

/*void DandyImg::getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp) {
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
} */

}
