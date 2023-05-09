#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "util/TOpt.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
//#include "psx/PsxBitmap.h"
//#include "psx/PsxPalette.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
//using namespace Psx;

const static int numEntries = 0x21;
const static int tileW = 8;
const static int tileH = 8;
const static int bytesPerString = 8;
const static int tilesPerRow = 16;
const static int tileOffset = 0x10;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}



void patchTile(TStream& ifs, int x, int y, int value) {
  ifs.seek(0x4);
  int w = ifs.readu8();
  ifs.seek(8 + (w * y * 2) + (x * 2));
  ifs.writeu16le(value);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Yuna 3 MS53 background patcher" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <infile> <outfile>" << std::endl;
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outFile(argv[2]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  patchTile(ifs, 208 / 8, 88 / 8, 0x5C);
  patchTile(ifs, 160 / 8, 112 / 8, 0x5C);
  // this requires a new tile, added at index 0x72
//  patchTile(ifs, 272 / 8, 88 / 8, 0x67);
//  patchTile(ifs, 96 / 8, 112 / 8, 0x67);
  patchTile(ifs, 272 / 8, 88 / 8, 0x72);
  patchTile(ifs, 96 / 8, 112 / 8, 0x72);
  // new tile
  patchTile(ifs, 208 / 8, 88 / 8, 0x73);
  patchTile(ifs, 160 / 8, 112 / 8, 0x73);
  
  ifs.save(outFile.c_str());
  
  return 0;
}
