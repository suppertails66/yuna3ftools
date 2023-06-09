#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxBitmap.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;
//using namespace Discaster;
using namespace Psx;

string as3bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 3) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

PsxBitmap::Format getFormatOfNumber(int num) {
  switch (num) {
  case 16:
    return PsxBitmap::format_rgb;
    break;
  case 2:
    return PsxBitmap::format_2bit;
    break;
  case 4:
    return PsxBitmap::format_4bit;
    break;
  case 8:
    return PsxBitmap::format_8bit;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "getFormatOfNumber()",
                            std::string("Unknown format code: ")
                            + TStringConversion::intToString(num));
    break;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 6) {
    cout << "PlayStation raw image converter" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>"
      << " <w> <h> <bpp> [options]" << endl;
    cout << "Supported BPP values are 2, 4, 8, and 16 (RGB)." << endl;
    cout << "Options:" << endl;
    cout << "  -i   Ignore first N bytes of input" << endl;
    
    return 0;
  }
  
  string inFile(argv[1]);
  string outFile(argv[2]);
  int w = TStringConversion::stringToInt(std::string(argv[3]));
  int h = TStringConversion::stringToInt(std::string(argv[4]));
  int bpp = TStringConversion::stringToInt(std::string(argv[5]));
  
  int initialOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-i", &initialOffset);
  
  PsxBitmap::Format inFormat = getFormatOfNumber(bpp);
  
  PsxBitmap img;
  
  // read image
//  img.setFormat(inFormat);
  {
    TBufStream ifs;
    ifs.open(inFile.c_str());
    ifs.seekoff(initialOffset);
    img.readPixelData(ifs, inFormat, w, h);
    
    TGraphic grp;
    if (inFormat == PsxBitmap::format_rgb) {
      img.toGraphic(grp, true, false);
    }
    else {
      img.toGrayscaleGraphic(grp);
    }
    TPngConversion::graphicToRGBAPng(outFile, grp);
  }
  
  return 0;
}
