#ifndef PSXCOLOR_H
#define PSXCOLOR_H


#include "util/TStream.h"
#include "util/TColor.h"

namespace Psx {


class PsxColor {
public:
  PsxColor();
  
  int nativeR() const;
  void setNativeR(int nativeR);
  int nativeG() const;
  void setNativeG(int nativeG);
  int nativeB() const;
  void setNativeB(int nativeB);
  bool semiTransparent() const;
  void setSemiTransparent(bool semiTransparent__);
  
  int asNativeColor() const;
  void fromNativeColor(int nativeColor);
  
  BlackT::TColor asTColor() const;
  void fromTColor(BlackT::TColor color);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
protected:
  // multiply native color by this to get full 8-bit level
  const static double nativeToFullColorScale;
  
  int r_;
  int g_;
  int b_;
  bool semiTransparent_;
};


}


#endif
