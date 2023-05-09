#include "psx/PsxPalette.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/MiscMath.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


PsxPalette::PsxPalette()
  { }

bool PsxPalette::hasColorAtIndex(int index) const {
  if (colorMap.find(index) != colorMap.end()) return true;
  return false;
}

PsxColor PsxPalette::getColorAtIndex(int index) const {
  return colorMap.at(index);
}

void PsxPalette::setColor(int index, PsxColor color) {
  colorMap[index] = color;
}

int PsxPalette::findMatchingColor(PsxColor color) const {
  // FIXME: how to handle semi-transparency flag?
  int nativeColor = color.asNativeColor() & 0x7FFF;
  for (PsxColorMap::const_iterator it = colorMap.cbegin();
       it != colorMap.cend();
       ++it) {
    if ((it->second.asNativeColor() & 0x7FFF) == nativeColor)
      return it->first;
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "PsxPalette::findMatchingColor()",
                          std::string("Unable to match color: ")
                            + TStringConversion::intToString(nativeColor,
                                TStringConversion::baseHex));
}

int PsxPalette::findMatchingColorOpaque(PsxColor color) const {
  // for opaque transfers, 0x8000 = nontransparent
  int nativeColor = color.asNativeColor();
  for (PsxColorMap::const_iterator it = colorMap.cbegin();
       it != colorMap.cend();
       ++it) {
    if ((it->second.asNativeColor()) == nativeColor)
      return it->first;
  }
  
  throw TGenericException(T_SRCANDLINE,
                          "PsxPalette::findMatchingColorOpaque()",
                          std::string("Unable to match color: ")
                            + TStringConversion::intToString(nativeColor,
                                TStringConversion::baseHex));
}

void PsxPalette::read(BlackT::TStream& ifs, int numColors) {
  for (int i = 0; i < numColors; i++) {
    PsxColor color;
    color.read(ifs);
    colorMap[i] = color;
  }
}

void PsxPalette::write(BlackT::TStream& ofs, int numColors) const {
  for (int i = 0; i < numColors; i++) {
    PsxColorMap::const_iterator it = colorMap.find(i);
    if (it != colorMap.end()) it->second.write(ofs);
    else {
      PsxColor color;
      color.write(ofs);
    }
  }
}


}
