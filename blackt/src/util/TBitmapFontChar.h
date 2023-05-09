#ifndef TBITMAPFONTCHAR_H
#define TBITMAPFONTCHAR_H


#include "util/TStream.h"
#include "util/TGraphic.h"
#include "util/TIniFile.h"
#include <string>

namespace BlackT {


class TBitmapFontChar {
public:
  friend class TBitmapFont;
  
  TBitmapFontChar();
  
  void readFromIni(TIniFile& ini, std::string& section);
  
  TGraphic grp;
  int glyphWidth;
  int advanceWidth;
  int vShift;
  bool noKerningIfFirst;
  bool noKerningIfSecond;
  // push this character X pixels left if second in pair
  int extraKerningIfRight;
  // push other character X pixels left if this is the first in the pair
  int extraKerningIfLeft;
protected:
//  bool has_firstKerningAgainstShortChars;
//  int firstKerningAgainstShortChars;
//  bool has_secondKerningAgainstShortChars;
//  int secondKerningAgainstShortChars;
  
};


}


#endif
