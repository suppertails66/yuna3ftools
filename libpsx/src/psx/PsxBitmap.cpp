#include "psx/PsxBitmap.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/MiscMath.h"
#include "util/TPngConversion.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


PsxBitmap::PsxBitmap()
  { }

void PsxBitmap::setFormat(Format format__) {
  format_ = format__;
}

void PsxBitmap::readPixelData(BlackT::TStream& ifs,
                              Format format__, int w, int h) {
  format_ = format__;
  
  switch (format_) {
  case format_4bit:
    if ((w % 4) != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "PsxBitmap::readPixelData()",
                              "4-bit bitmap width not a multiple of 4");
    }
    break;
  case format_8bit:
    if ((w % 2) != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "PsxBitmap::readPixelData()",
                              "8-bit bitmap width not a multiple of 2");
    }
    break;
  case format_2bit:
    if ((w % 8) != 0) {
      throw TGenericException(T_SRCANDLINE,
                              "PsxBitmap::readPixelData()",
                              "2-bit bitmap width not a multiple of 8");
    }
    break;
  default:
    break;
  }
  
  data_.resize(w, h);
  
  switch (format_) {
  case format_4bit:
    readPixelData4Bit(ifs);
    break;
  case format_8bit:
    readPixelData8Bit(ifs);
    break;
  case format_rgb:
    readPixelDataRgb(ifs);
    break;
  case format_2bit:
    readPixelData2Bit(ifs);
    break;
  default:
    break;
  }
}

void PsxBitmap::writePixelData(BlackT::TStream& ofs) const {
  switch (format_) {
  case format_4bit:
    writePixelData4Bit(ofs);
    break;
  case format_8bit:
    writePixelData8Bit(ofs);
    break;
  case format_rgb:
    writePixelDataRgb(ofs);
    break;
  case format_2bit:
    writePixelData2Bit(ofs);
    break;
  default:
    break;
  }
}

void PsxBitmap::setPalette(const PsxPalette& palette__) {
  palette_ = palette__;
}

PsxPalette PsxBitmap::palette() const {
  return palette_;
}

void PsxBitmap::toGraphic(BlackT::TGraphic& dst,
                          bool transparency,
                          bool semiTransparency) const {
  dst.resize(data_.w(), data_.h());  
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      PsxColor nativeColor;
      
      switch (format_) {
      case format_4bit:
      case format_8bit:
      case format_2bit:
        nativeColor = palette_.getColorAtIndex(data_.data(i, j));
        break;
      case format_rgb:
        nativeColor.fromNativeColor(data_.data(i, j));
        break;
      default:
        break;
      }
      
      TColor realColor = nativeColor.asTColor();
      
      // map non-transparent black
      if (transparency
          && (nativeColor.asNativeColor() == 0x8000)) {
        realColor.setA(TColor::fullAlphaOpacity);
      }
      
      if (!transparency
          && (realColor.a() == TColor::fullAlphaTransparency)) {
        realColor.setA(TColor::fullAlphaOpacity);
      }
      
      if (!semiTransparency
          && (realColor.a() != TColor::fullAlphaTransparency)
          && (realColor.a() != TColor::fullAlphaOpacity)) {
        realColor.setA(TColor::fullAlphaOpacity);
      }
      
      dst.setPixel(i, j, realColor);
    }
  }
}

void PsxBitmap::toGrayscaleGraphic(BlackT::TGraphic& dst) const {
  if (format_ == format_rgb) {
    throw TGenericException(T_SRCANDLINE,
                            "PsxBitmap::toGrayscaleGraphic()",
                            "Cannot render RGB bitmap as grayscale");
  }
  
  dst.resize(data_.w(), data_.h());
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      int raw = data_.data(i, j);
      int level = 0;
      
      switch (format_) {
      case format_4bit:
        level = (raw | (raw << 4));
        break;
      case format_8bit:
        level = raw;
        break;
      case format_2bit:
        level = (raw | (raw << 2) | (raw << 4) | (raw << 6));
        break;
      default:
        break;
      }
      
      dst.setPixel(i, j,
                   TColor(level, level, level, TColor::fullAlphaOpacity));
    }
  }
}

void PsxBitmap::fromGraphic(BlackT::TGraphic& src,
                            bool transparency) {
  data_.resize(src.w(), src.h());
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      try {
        TColor realColor = src.getPixel(i, j);
        
        if (format_ == format_rgb) {
          // TODO: transparency?
          PsxColor nativeColor;
          nativeColor.fromTColor(realColor);
          nativeColor.setSemiTransparent(false);
          data_.data(i, j) = nativeColor.asNativeColor();
          continue;
        }
        
        // FIXME: this probably needs to be more general
        
        // if transparency commands are used:
        // - pure black with no semitransparent flag = fully transparent
        // - pure black with semitransparent flag = semitransparent
        // if opaque commands are used:
        // - pure black with no semitransparent flag = fully transparent
        // - pure black with semitransparent flag = fully opaque
        
        int index = 0;
//        PsxColor nativeColor;
//        nativeColor.fromTColor(realColor);
//        index = palette_.findMatchingColor(nativeColor);
        
        if (transparency) {
          PsxColor nativeColor;
          if (realColor.a() == TColor::fullAlphaTransparency) {
            // transparency must map to pure black with no semitransparent flag
            nativeColor.fromTColor(TColor(0, 0, 0, TColor::fullAlphaOpacity));
            nativeColor.setSemiTransparent(false);
            index = palette_.findMatchingColorOpaque(nativeColor);
          }
          else {
            nativeColor.fromTColor(realColor);
            
            // nontransparent black must map to 0x8000 (semitransparency flag).
            // for anything else, the flag doesn't matter.
            if (nativeColor.asNativeColor() == 0x0000) {
              nativeColor.setSemiTransparent(true);
              index = palette_.findMatchingColorOpaque(nativeColor);
            }
            else {
              index = palette_.findMatchingColor(nativeColor);
            }
          }
        }
        else {
          PsxColor nativeColor;
          nativeColor.fromTColor(realColor);
          index = palette_.findMatchingColor(nativeColor);
        }
        
        data_.data(i, j) = index;
      }
      catch (std::exception& e) {
        std::cerr << "Error processing pixel "
          << "(" << i << ", " << j << "): "
          << e.what()
          << std::endl;
        throw e;
      }
    }
  }
}

void PsxBitmap::fromGrayscaleGraphic(BlackT::TGraphic& src) {
  if (format_ == format_rgb) {
    throw TGenericException(T_SRCANDLINE,
                            "PsxBitmap::fromGrayscaleGraphic()",
                            "Cannot convert grayscale image to RGB bitmap");
  }
  
  data_.resize(src.w(), src.h());
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i++) {
      int level = src.getPixel(i, j).r();
      int raw = 0;
      
      switch (format_) {
      case format_4bit:
        raw = (level & (0x0F << 4)) >> 4;
        break;
      case format_8bit:
        raw = (level & (0xFF << 0)) >> 0;
        break;
      case format_2bit:
        raw = (level & (0x03 << 6)) >> 6;
        break;
      default:
        break;
      }
      
      data_.data(i, j) = raw;
    }
  }
}

int PsxBitmap::outputDataSize() const {
  int baseSize = data_.w() * data_.h();
  
  switch (format_) {
  case format_4bit:
    return baseSize / 2;
    break;
  case format_8bit:
    return baseSize;
    break;
  case format_rgb:
    return baseSize * 2;
    break;
  case format_2bit:
    return baseSize / 4;
    break;
  default:
    break;
  }
  
  return 0;
}

void PsxBitmap::readPixelData4Bit(BlackT::TStream& ifs) {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 4) {
      int next = ifs.readu16le();
      
      data_.data(i + 0, j) = (next & (0x0F << 0)) >> 0;
      data_.data(i + 1, j) = (next & (0x0F << 4)) >> 4;
      data_.data(i + 2, j) = (next & (0x0F << 8)) >> 8;
      data_.data(i + 3, j) = (next & (0x0F << 12)) >> 12;
    }
  }
}

void PsxBitmap::readPixelData8Bit(BlackT::TStream& ifs) {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 2) {
      int next = ifs.readu16le();
      
      data_.data(i + 0, j) = (next & (0xFF << 0)) >> 0;
      data_.data(i + 1, j) = (next & (0xFF << 8)) >> 8;
    }
  }
}

void PsxBitmap::readPixelDataRgb(BlackT::TStream& ifs) {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 1) {
      int next = ifs.readu16le();
      
      data_.data(i, j) = (next & (0xFFFF << 0)) >> 0;
    }
  }
}

void PsxBitmap::readPixelData2Bit(BlackT::TStream& ifs) {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 8) {
      int next = ifs.readu16le();
      
      data_.data(i + 0, j) = (next & (0x03 << 0)) >> 0;
      data_.data(i + 1, j) = (next & (0x03 << 2)) >> 2;
      data_.data(i + 2, j) = (next & (0x03 << 4)) >> 4;
      data_.data(i + 3, j) = (next & (0x03 << 6)) >> 6;
      data_.data(i + 4, j) = (next & (0x03 << 8)) >> 8;
      data_.data(i + 5, j) = (next & (0x03 << 10)) >> 10;
      data_.data(i + 6, j) = (next & (0x03 << 12)) >> 12;
      data_.data(i + 7, j) = (next & (0x03 << 14)) >> 14;
    }
  }
}

void PsxBitmap::writePixelData4Bit(BlackT::TStream& ofs) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 4) {
      int next = 0;
      
      next |= (data_.data(i + 0, j) << 0);
      next |= (data_.data(i + 1, j) << 4);
      next |= (data_.data(i + 2, j) << 8);
      next |= (data_.data(i + 3, j) << 12);
      
      ofs.writeu16le(next);
    }
  }
}

void PsxBitmap::writePixelData8Bit(BlackT::TStream& ofs) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 2) {
      int next = 0;
      
      next |= (data_.data(i + 0, j) << 0);
      next |= (data_.data(i + 1, j) << 8);
      
      ofs.writeu16le(next);
    }
  }
}

void PsxBitmap::writePixelDataRgb(BlackT::TStream& ofs) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 1) {
      int next = 0;
      
      next |= (data_.data(i + 0, j) << 0);
      
      ofs.writeu16le(next);
    }
  }
}

void PsxBitmap::writePixelData2Bit(BlackT::TStream& ofs) const {
  for (int j = 0; j < data_.h(); j++) {
    for (int i = 0; i < data_.w(); i += 8) {
      int next = 0;
      
      next |= (data_.data(i + 0, j) << 0);
      next |= (data_.data(i + 1, j) << 2);
      next |= (data_.data(i + 2, j) << 4);
      next |= (data_.data(i + 3, j) << 6);
      next |= (data_.data(i + 4, j) << 8);
      next |= (data_.data(i + 5, j) << 10);
      next |= (data_.data(i + 6, j) << 12);
      next |= (data_.data(i + 7, j) << 14);
      
      ofs.writeu16le(next);
    }
  }
}

int PsxBitmap::w() const {
  return data_.w();
}

int PsxBitmap::h() const {
  return data_.h();
}

//void PsxBitmap::read(BlackT::TStream& ifs) {
//  
//}

//void PsxBitmap::write(BlackT::TStream& ofs) const {
//  
//}


}
