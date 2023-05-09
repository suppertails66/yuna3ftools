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



int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Yuna 3 STWIN.GS8 string preview generator" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <headered_exe> <stwin_img> <out_img>" << std::endl;
    return 0;
  }
  
  std::string exeFileName(argv[1]);
  std::string stwinFileName(argv[2]);
  std::string outFileName(argv[3]);
  
  TGraphic stwinGrp;
  TPngConversion::RGBAPngToGraphic(stwinFileName, stwinGrp);
  
  TGraphic outGrp(tileW * bytesPerString, tileH * numEntries);
  outGrp.clearTransparent();
  
  TBufStream ifs;
  ifs.open(exeFileName.c_str());
  for (int i = 0; i < numEntries; i++) {
    int basePos = (0x801B2F40 - 0x80110000 + 0x800) + (i * bytesPerString);
//    int basePos = (0x801B2F40 - 0x80010000 + 0x800) + (i * bytesPerString);
    ifs.seek(basePos);
    
    int baseY = i * tileH;
    for (int j = 0; j < bytesPerString; j++) {
      int tileId = ifs.readu8();
      if (tileId == 0) break;
      
      tileId -= tileOffset;
      if (tileId < 0) break;
      
      int srcX = (tileId % tilesPerRow) * tileW;
      int srcY = (tileId / tilesPerRow) * tileH;
      int dstX = j * tileW;
      int dstY = baseY;
      outGrp.copy(stwinGrp,
                  TRect(dstX, dstY, tileW, tileH),
                  TRect(srcX, srcY, tileW, tileH));
    }
    
    ifs.seek(basePos);
    std::cout << ".db ";
    for (int j = 0; j < 8; j++) {
      if (j != 0) cout << ",";
      cout << "0x" << as2bHex(ifs.readu8());
    }
    std::cout << std::endl;
  }
  
  TPngConversion::graphicToRGBAPng(outFileName, outGrp);
  
  return 0;
}
