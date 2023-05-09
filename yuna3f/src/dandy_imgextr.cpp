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
#include "dandy/DandyImg.h"
#include "dandy/DandyImgExtr.h"
#include "psx/PsxPalette.h"
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

int fontCharW = 16;
int fontCharH = 16;
int bytesPerFontChar = (fontCharW * fontCharH) / 2;

int charactersPerRow = 16;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Remote Control Dandy image extractor" << endl;
    cout << "Usage: " << argv[0] << " [image] [outprefix]" << endl;
    cout << "Options: " << endl;
    cout << "  -grayscale            Force grayscale output even if a palette exists"
      << endl;
    cout << "  -transparency         Enable transparency (disabled by default)"
      << endl;
    cout << "  -semitransparency     Enable semitransparency (disabled by default)"
      << endl;
    cout << "  -forcepalette [file]  Force use of the specified palette"
      << endl;
    cout << "  -offset [file]        Offset starting position by X bytes"
      << endl;
    
    return 0;
  }
  
  string imageName(argv[1]);
  string outName(argv[2]);
  
  bool grayscale = TOpt::hasFlag(argc, argv, "-grayscale");
  bool transparency = TOpt::hasFlag(argc, argv, "-transparency");
  bool semiTransparency = TOpt::hasFlag(argc, argv, "-semitransparency");
  
  char* forcedPalName = TOpt::getOpt(argc, argv, "-forcepalette");
  
  int startOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-offset", &startOffset);
  
  TBufStream ifs;
  ifs.open(imageName.c_str());
  
//  DandyImgExtr::extract(ifs, outName);

  DandyImg img;
  
  img.setGrayscale(grayscale);
  img.setTransparency(transparency);
  img.setSemiTransparency(semiTransparency);
  
  ifs.seek(startOffset);
  img.read(ifs);
  
  if (forcedPalName != NULL) {
    TBufStream ifs;
    ifs.open(forcedPalName);
    PsxPalette pal;
    pal.read(ifs, ifs.size() / 2);
    img.setForcedPalette(pal);
  }
  
  img.save(outName);
  
  return 0;
}
