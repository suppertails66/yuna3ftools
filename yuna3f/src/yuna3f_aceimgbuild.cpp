#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "util/TOpt.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "psx/PsxBitmap.h"
#include "psx/PsxPalette.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
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

string as1bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 1) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cout << "Yuna 3 ACE image builder" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <imgfile> <tcofile> <gs8file> <outfile> [options]" << std::endl;
    std::cout << "Options: "
      << "  --notrans   Disable transparency" << std::endl;
    return 0;
  }
  
  std::string imgFileName(argv[1]);
  std::string tcoFileName(argv[2]);
  std::string gs8FileName(argv[3]);
  std::string outFileName(argv[4]);
  
  bool transparency = true;
  if (TOpt::hasFlag(argc, argv, "--notrans")) transparency = false;
  
  PsxPalette pal;
  {
    TBufStream ifs;
    ifs.open(tcoFileName.c_str());
    ifs.seek(0x100);
    
    // endianness is reversed
    for (int i = 0; i < 256; i++) {
      int next = ifs.readu16be();
      ifs.seekoff(-2);
      ifs.writeu16le(next);
    }
    
    ifs.seek(0x100);
    pal.read(ifs, 256);
  }
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(imgFileName, grp);
  
  PsxBitmap img;
  img.setPalette(pal);
  img.setFormat(PsxBitmap::format_8bit);
  img.fromGraphic(grp, transparency);
  
  TBufStream ifs;
  ifs.open(gs8FileName.c_str());
  
  // update w/h fields
  ifs.seek(0x36);
  ifs.writeu16le(img.w());
  ifs.writeu16le(img.h());
  
  // update pixel data
  ifs.seek(0x100);
  img.writePixelData(ifs);
  
  TFileManip::createDirectoryForFile(outFileName);
  ifs.save(outFileName.c_str());
  
  return 0;
}
