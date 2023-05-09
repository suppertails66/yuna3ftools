#include "dandy/DandyLinkImg.h"
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


#define DANDYLINKIMG_SAVE_INT_MACRO(INI, NAME) \
  INI.setValue("Settings",\
               #NAME,\
               TStringConversion::intToString(NAME))

#define DANDYLINKIMG_LOAD_INT_MACRO(INI, NAME) \
  NAME = TStringConversion::stringToInt(INI.valueOfKey("Settings",\
               #NAME))


DandyLinkImg::DandyLinkImg()
  : format(format_4bit),
//    unk0(0),
    unk1(0),
    unk2(0),
    unk3(0),
    unk4(0),
    unk5(0),
//    numPalettes(0),
    transparency(false),
    grayscale(false),
    semiTransparency(false)
    { }

void DandyLinkImg::setTransparency(bool transparency__) {
  transparency = transparency__;
}

void DandyLinkImg::setSemiTransparency(bool semiTransparency__) {
  semiTransparency = semiTransparency__;
}

void DandyLinkImg::setGrayscale(bool grayscale__) {
  grayscale = grayscale__;
}

void DandyLinkImg::read(TStream& ifs) {
  int bitsPerPixel = ifs.readu32le();
  
  unk1 = ifs.readu16le();
  unk2 = ifs.readu16le();
  int pitch = ifs.readu16le();
  int height = ifs.readu16le();
  unk3 = ifs.readu16le();
  unk4 = ifs.readu16le();
  unk5 = ifs.readu16le();
//  unk6 = ifs.readu16le();
  int numPalettes = ifs.readu16le();
  
  // always?
  switch (bitsPerPixel) {
  case 4:
    format = format_4bit;
    break;
  case 8:
    format = format_8bit;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyLinkImg::read()",
                            std::string("Unknown bitsPerPixel: ")
                              + TStringConversion::intToString(bitsPerPixel));
    break;
  }
  
  // always?
//  hasPalette = true;
  
  int numColors = 0;
//  if (numPalettes > 0) {
    if (format == format_8bit) {
      // 8-bit
      numColors = 256;
    }
    else {
      // always use 16-color palette, even in 2-bit mode
      numColors = 16;
    }
//  }
  
  // palette
  palettes.resize(numPalettes);
  if (numPalettes > 0) {
    for (int j = 0; j < numPalettes; j++) {
      PsxPalette palette;
      for (int i = 0; i < numColors; i++) {
        PsxColor color;
        color.read(ifs);
        
  //      color.setSemiTransparent(false);
        
        palette.setColor(i, color);
      }
      palettes[j] = palette;
    }
    
    // use first palette as "canon"
    bitmap.setPalette(palettes[0]);
  }
  
  int width = pitch;
  switch (format) {
//  case format_2bit:
//    width *= 8;
//    break;
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
  
//  bitmap.setPalette(palette);
  
  // read pixel data according to format
  switch (format) {
//  case format_2bit:
//    bitmap.readPixelData(ifs, PsxBitmap::format_2bit, width, height);
//    break;
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

void DandyLinkImg::save(std::string outprefix) const {
  
  // metadata
  
  {
    TIniFile ini;
//    ini.setValue("Settings",
//                 "headerUnk",
//                 TStringConversion::intToString(headerUnk));
    DANDYLINKIMG_SAVE_INT_MACRO(ini, format);
//    DANDYLINKIMG_SAVE_INT_MACRO(ini, hasPalette);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, transparency);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, semiTransparency);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, grayscale);
    
//    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk0);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk1);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk2);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk3);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk4);
    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk5);
//    DANDYLINKIMG_SAVE_INT_MACRO(ini, unk6);
    ini.setValue("Settings",
                 "numPalettes",
                 TStringConversion::intToString(palettes.size()));
    
    ini.writeFile(outprefix + "-data.txt");
  }
  
  // palette
  
  if (palettes.size() > 0) {
    for (int j = 0; j < palettes.size(); j++) {
      TBufStream ofs;
      
      const PsxPalette& palette = palettes[j];
      switch (format) {
  //    case format_2bit:
      case format_4bit:
        palette.write(ofs, 16);
        break;
      case format_8bit:
        palette.write(ofs, 256);
        break;
      default:
        break;
      }
      
      ofs.save(
        (outprefix + "-pal-" + TStringConversion::intToString(j) + ".bin")
        .c_str());
    }
  }

  // image data
  
  {
    TGraphic grp;
    
    if ((palettes.size() == 0) || grayscale) {
      bitmap.toGrayscaleGraphic(grp);
    }
    else {
      bitmap.toGraphic(grp, transparency, semiTransparency);
    }
    
    TPngConversion::graphicToRGBAPng(outprefix + "-img.png", grp);
  }
}

void DandyLinkImg::load(std::string inprefix) {
  
  // metadata
  
  int numPalettes = 0;
  {
    TIniFile ini;
    ini.readFile(inprefix + "-data.txt");
    
    format = static_cast<Format>(
      TStringConversion::stringToInt(ini.valueOfKey("Settings", "format"))
      );
//    DANDYLINKIMG_LOAD_INT_MACRO(ini, hasPalette);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, transparency);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, semiTransparency);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, grayscale);
    
//    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk0);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk1);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk2);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk3);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk4);
    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk5);
//    DANDYLINKIMG_LOAD_INT_MACRO(ini, unk6);
    numPalettes = TStringConversion::stringToInt(
      ini.valueOfKey("Settings", "numPalettes"));
  }
  
  // palette
  
  palettes.resize(numPalettes);
  if (palettes.size() > 0) {
    for (int j = 0; j < palettes.size(); j++) {
      TBufStream ifs;
      ifs.open(
        (inprefix + "-pal-" + TStringConversion::intToString(j) + ".bin")
        .c_str());
      
      PsxPalette palette;
      switch (format) {
  //    case format_2bit:
      case format_4bit:
        palette.read(ifs, 16);
        break;
      case format_8bit:
        palette.read(ifs, 256);
        break;
      default:
        break;
      }
      
      palettes[j] = palette;
    }
    
    // palette 0 = main
    bitmap.setPalette(palettes[0]);
  }

  // image data
  
  {
    TGraphic grp;
    TPngConversion::RGBAPngToGraphic(inprefix + "-img.png", grp);
    
    switch (format) {
//    case format_2bit:
//      bitmap.setFormat(PsxBitmap::format_2bit);
//      break;
    case format_4bit:
      bitmap.setFormat(PsxBitmap::format_4bit);
      break;
    case format_8bit:
      bitmap.setFormat(PsxBitmap::format_8bit);
      break;
    default:
      break;
    }
    
    if ((palettes.size() == 0) || grayscale) {
      bitmap.fromGrayscaleGraphic(grp);
    }
    else {
      bitmap.fromGraphic(grp, transparency);
    }
  }
}

void DandyLinkImg::write(BlackT::TStream& ofs) const {
//  ofs.writeu32le(unk0);
//  std::cerr << std::hex << unk0 << " " << ofs.tell() << std::endl;

  switch (format) {
//  case format_2bit:
//    break;
  case format_4bit:
    ofs.writeu32le(4);
    break;
  case format_8bit:
    ofs.writeu32le(8);
    break;
  default:
    break;
  }

  ofs.writeu16le(unk1);
  ofs.writeu16le(unk2);
  
  int width = bitmap.w();
  int height = bitmap.h();
  
  int pitch = width;
  switch (format) {
//  case format_2bit:
//    pitch /= 8;
//    break;
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
  
  ofs.writeu16le(unk3);
  ofs.writeu16le(unk4);
  ofs.writeu16le(unk5);
//  ofs.writeu16le(unk6);
  ofs.writeu16le(palettes.size());
  
  int numColors = 0;
//  if (palettes.size() > 0) {
  if (format == format_8bit) {
    // 8-bit
    numColors = 256;
  }
  else {
    // always use 16-color palette, even in 2-bit mode
    numColors = 16;
  }
//  }
  
  if (palettes.size() > 0) {
//    bitmap.palette().write(ofs, numColors);
    for (int j = 0; j < palettes.size(); j++) {
      palettes[j].write(ofs, numColors);
    }
  }
  
  bitmap.writePixelData(ofs);
}

}
