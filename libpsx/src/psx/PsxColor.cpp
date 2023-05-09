#include "psx/PsxColor.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/MiscMath.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


const double PsxColor::nativeToFullColorScale = (double)255 / (double)31;

PsxColor::PsxColor()
  : r_(0), g_(0), b_(0), semiTransparent_(false) { }

int PsxColor::nativeR() const {
  return r_;
}

void PsxColor::setNativeR(int nativeR) {
  r_ = nativeR & 0x1F;
}

int PsxColor::nativeG() const {
  return g_;
}

void PsxColor::setNativeG(int nativeG) {
  g_ = nativeG & 0x1F;
}

int PsxColor::nativeB() const {
  return b_;
}

void PsxColor::setNativeB(int nativeB) {
  b_ = nativeB & 0x1F;
}

bool PsxColor::semiTransparent() const {
  return semiTransparent_;
}

void PsxColor::setSemiTransparent(bool semiTransparent__) {
  semiTransparent_ = semiTransparent__;
}

int PsxColor::asNativeColor() const {
  int value = (r_ << 0) | (g_ << 5) | (b_ << 10);
  if (semiTransparent_) value |= 0x8000;
  return value;
}

void PsxColor::fromNativeColor(int nativeColor) {
  r_ = (nativeColor & (0x1F << 0)) >> 0;
  g_ = (nativeColor & (0x1F << 5)) >> 5;
  b_ = (nativeColor & (0x1F << 10)) >> 10;
  semiTransparent_ = ((nativeColor & 0x8000) != 0);
}

BlackT::TColor PsxColor::asTColor() const {
  // all components zero == fully transparent
  if ((r_ == 0) && (g_ == 0) && (b_ == 0)
      && (!semiTransparent_)) {
    return TColor(0, 0, 0, TColor::fullAlphaTransparency);
  }
  
  TColor result;
  result.setR(r_ * nativeToFullColorScale);
  result.setG(g_ * nativeToFullColorScale);
  result.setB(b_ * nativeToFullColorScale);
  if (semiTransparent_) result.setA(128);
  else result.setA(TColor::fullAlphaOpacity);
  
  return result;
}

void PsxColor::fromTColor(BlackT::TColor color) {
  // fully transparent == all components zero
  if (color.a() == TColor::fullAlphaTransparency) {
    r_ = 0;
    g_ = 0;
    b_ = 0;
    semiTransparent_ = false;
    return;
  }
  
//  r_ = (int)(color.r() / nativeToFullColorScale) & 0x1F;
//  g_ = (int)(color.g() / nativeToFullColorScale) & 0x1F;
//  b_ = (int)(color.b() / nativeToFullColorScale) & 0x1F;
  r_ = (int)(color.r() >> 3) & 0x1F;
  g_ = (int)(color.g() >> 3) & 0x1F;
  b_ = (int)(color.b() >> 3) & 0x1F;
  // treat anything not fully opaque as semi-transparent
  semiTransparent_ = (color.a() != TColor::fullAlphaOpacity);
}

void PsxColor::read(BlackT::TStream& ifs) {
  fromNativeColor(ifs.readu16le());
}

void PsxColor::write(BlackT::TStream& ofs) const {
  ofs.writeu16le(asNativeColor());
}


}
