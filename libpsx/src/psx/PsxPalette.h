#ifndef PSXPALETTE_H
#define PSXPALETTE_H


#include "psx/PsxColor.h"
#include "util/TStream.h"
#include <map>

namespace Psx {


class PsxPalette {
public:
  PsxPalette();
  
  bool hasColorAtIndex(int index) const;
  PsxColor getColorAtIndex(int index) const;
  void setColor(int index, PsxColor color);
  
  int findMatchingColor(PsxColor color) const;
  int findMatchingColorOpaque(PsxColor color) const;
  
  void read(BlackT::TStream& ifs, int numColors);
  void write(BlackT::TStream& ofs, int numColors) const;
  
protected:
  typedef std::map<int, PsxColor> PsxColorMap;
  
  PsxColorMap colorMap;
};


}


#endif
