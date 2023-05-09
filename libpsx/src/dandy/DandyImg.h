#ifndef DANDYIMG_H
#define DANDYIMG_H


#include "util/TStream.h"
#include "util/TGraphic.h"
#include "psx/PsxBitmap.h"
#include <string>

namespace Psx {


class DandyImg {
public:
  enum Format {
    format_2bit,
    format_4bit,
    format_8bit
  };
  
  DandyImg();
  
  void setTransparency(bool transparency__);
  void setSemiTransparency(bool semiTransparency__);
  void setGrayscale(bool grayscale__);
  void setForcedPalette(const PsxPalette& palette);
  void stripPalette();
  
  void read(BlackT::TStream& ifs);
  void save(std::string outprefix) const;
  void load(std::string inprefix);
  void write(BlackT::TStream& ofs) const;
  
//  static void getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp);
  
protected:
  
  // first word of file.
  // seems to always be 0x10?
  // possibly header size, but doesn't seem to match...
  int headerUnk;
  int headerUnk2;
  int headerUnk3;
  
  Format format;
  PsxBitmap bitmap;
  
  // unknown bit 1 of format field
  // set in e.g. SINKOU.PAC-0 (title frontend resources)
  bool formatFlag1;
  // bit 3 of format
  bool hasPalette;
  
  int paletteUnk0;
  int paletteUnk2;
  int paletteUnk3;
  
  int gpuDstLo;
  int gpuDstHi;
  
  // not in header (probably)
  // for use when exporting graphic
  bool transparency;
  bool grayscale;
  bool semiTransparency;
  
};


}


#endif
