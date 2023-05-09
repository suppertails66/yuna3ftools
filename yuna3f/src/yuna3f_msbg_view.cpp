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



const static int celW = 8;
const static int celH = 8;

struct Cel {
  int x;
  int y;
  int unk1;
  int unk2;
  int unk3;
  int unk4;
  int unk5;
  int unk6;
  
  void read(TStream& ifs) {
    x = ifs.readu8();
    y = ifs.readu8();
    unk1 = ifs.readu8();
    unk2 = ifs.readu8();
    unk3 = ifs.readu8();
    unk4 = ifs.readu8();
    unk5 = ifs.readu8();
    unk6 = ifs.readu8();
  }
};

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Yuna 3 mission background extractor" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <cel> <bgd> <outfile> [tileimg1] [tileimg2]" << std::endl;
    return 0;
  }
  
  std::string celName(argv[1]);
  std::string bgdName(argv[2]);
  std::string outFileName(argv[3]);
  
  std::string img1Name;
  if (argc >= 5) {
    img1Name = std::string(argv[4]);
  }
  
  std::string img2Name;
  if (argc >= 6) {
    img2Name = std::string(argv[5]);
  }
  
  TGraphic img1Grp;
  if (!img1Name.empty())
    TPngConversion::RGBAPngToGraphic(img1Name, img1Grp);
  TGraphic img2Grp;
  if (!img2Name.empty())
    TPngConversion::RGBAPngToGraphic(img2Name, img2Grp);
  
  std::vector<Cel> cels;
  {
    TBufStream celIfs;
    celIfs.open(celName.c_str());
    celIfs.seek(0x8);
    while (!celIfs.eof()) {
      Cel cel;
      cel.read(celIfs);
      cels.push_back(cel);
    }
  }
  
  TBufStream bgdIfs;
  bgdIfs.open(bgdName.c_str());
  int unk1 = bgdIfs.readu32le();
  int imgCelW = bgdIfs.readu8();
  int imgCelH = bgdIfs.readu8();
  int unk2 = bgdIfs.readu8();
  int unk3 = bgdIfs.readu8();
  
  TGraphic outGrp(imgCelW * celW, imgCelH * celH);
  for (int j = 0; j < imgCelH; j++) {
    for (int i = 0; i < imgCelW; i++) {
      int celNum = bgdIfs.readu16le();
      const Cel& cel = cels.at(celNum);
      int srcX = cel.x;
      int srcY = cel.y;
      int dstX = i * celW;
      int dstY = j * celH;
      
      TGraphic* srcGrp = NULL;
      if (cel.unk5 == 0x9E) srcGrp = &img2Grp;
      else srcGrp = &img1Grp;
      
      outGrp.copy(*srcGrp,
                  TRect(dstX, dstY, 0, 0),
                  TRect(srcX, srcY, 0, 0));
    }
  }
  
  
  TPngConversion::graphicToRGBAPng(outFileName, outGrp);
  
  return 0;
}
