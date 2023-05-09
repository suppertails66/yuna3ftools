#include "util/TBitmapFontChar.h"
#include "util/TPngConversion.h"
#include "util/TBufStream.h"

namespace BlackT {


static bool getIntIfExists(int* dst, const TIniFile& ini, std::string section,
                           std::string key) {
  if (dst == NULL) return false;
  if (!ini.hasSection(section)) return false;
  if (!ini.hasKey(section, key)) return false;
  
  *dst = TStringConversion::stringToInt(ini.valueOfKey(section, key));
  return true;
}

static bool getBoolIfExists(bool* dst, const TIniFile& ini, std::string section,
                     std::string key) {
  if (dst == NULL) return false;
  if (!ini.hasSection(section)) return false;
  if (!ini.hasKey(section, key)) return false;
  
  *dst = (TStringConversion::stringToInt(ini.valueOfKey(section, key)) != 0);
  return true;
}

TBitmapFontChar::TBitmapFontChar()
    : glyphWidth(-1),
      advanceWidth(-1),
      vShift(0),
      noKerningIfFirst(false),
      noKerningIfSecond(false),
      extraKerningIfRight(0),
      extraKerningIfLeft(0)
//      has_firstKerningAgainstShortChars(false),
//      firstKerningAgainstShortChars(0),
//      has_secondKerningAgainstShortChars(false),
//      secondKerningAgainstShortChars(0)
      { }

void TBitmapFontChar::readFromIni(TIniFile& ini, std::string& section) {
  if (!ini.hasSection(section)) return;
  
  getIntIfExists(&glyphWidth, ini, section, "glyphWidth");
  getIntIfExists(&advanceWidth, ini, section, "advanceWidth");
  getIntIfExists(&vShift, ini, section, "vShift");
  getBoolIfExists(&noKerningIfFirst, ini, section, "noKerningIfFirst");
  getBoolIfExists(&noKerningIfSecond, ini, section, "noKerningIfSecond");
  getIntIfExists(&extraKerningIfRight, ini, section, "extraKerningIfRight");
  getIntIfExists(&extraKerningIfLeft, ini, section, "extraKerningIfLeft");
/*  has_firstKerningAgainstShortChars = getIntIfExists(
    &firstKerningAgainstShortChars, ini, section,
    "firstKerningAgainstShortChars");
  has_secondKerningAgainstShortChars = getIntIfExists(
    &secondKerningAgainstShortChars, ini, section,
    "secondKerningAgainstShortChars"); */
}


}
