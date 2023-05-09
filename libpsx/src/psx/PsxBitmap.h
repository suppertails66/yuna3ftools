#ifndef PSXBITMAP_H
#define PSXBITMAP_H


#include "psx/PsxColor.h"
#include "psx/PsxPalette.h"
#include "util/TStream.h"
#include "util/TTwoDArray.h"
#include "util/TGraphic.h"
#include <map>

namespace Psx {


class PsxBitmap {
public:
  enum Format {
    format_4bit,
    format_8bit,
    format_rgb,
    // not a hardware format, but games use it...
    format_2bit
  };  
  
  PsxBitmap();
  
  void setFormat(Format format__);
  
  void readPixelData(BlackT::TStream& ifs,
                     Format format__, int w, int h);
  void writePixelData(BlackT::TStream& ofs) const;
  
  void setPalette(const PsxPalette& palette__);
  PsxPalette palette() const;
  
  void toGraphic(BlackT::TGraphic& dst,
                 bool transparency = true,
                 bool semiTransparency = true) const;
  void toGrayscaleGraphic(BlackT::TGraphic& dst) const;
  
  void fromGraphic(BlackT::TGraphic& src,
                   bool transparency = true);
  void fromGrayscaleGraphic(BlackT::TGraphic& src);
  
  int outputDataSize() const;
  int w() const;
  int h() const;
  
//  void read(BlackT::TStream& ifs);
//  void write(BlackT::TStream& ofs) const;
  
protected:
  typedef BlackT::TTwoDArray<int> PixelDataArray;
  
  Format format_;
  PixelDataArray data_;
  PsxPalette palette_;
  
  void readPixelData4Bit(BlackT::TStream& ifs);
  void readPixelData8Bit(BlackT::TStream& ifs);
  void readPixelDataRgb(BlackT::TStream& ifs);
  void readPixelData2Bit(BlackT::TStream& ifs);
  
  void writePixelData4Bit(BlackT::TStream& ofs) const;
  void writePixelData8Bit(BlackT::TStream& ofs) const;
  void writePixelDataRgb(BlackT::TStream& ofs) const;
  void writePixelData2Bit(BlackT::TStream& ofs) const;
};


}


#endif
