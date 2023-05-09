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
#include <map>
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
    std::cout << "Yuna 3 mission background unextractor" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <cel> <bgd> <infile> [options]" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "  --img1 <file>    Set output image 1" << std::endl;
    std::cout << "  --img2 <file>    Set output image 2" << std::endl;
    std::cout << "  --boundw <value> Limit width of cel processing" << std::endl;
    std::cout << "  --boundh <value> Limit height of cel processing" << std::endl;
    return 0;
  }
  
  std::string celName(argv[1]);
  std::string bgdName(argv[2]);
  std::string inFileName(argv[3]);
  
  std::string img1Name;
  {
    int argPos;
    if ((argPos = TOpt::findOpt(argc, argv, "--img1")) != -1)
      img1Name = std::string(argv[argPos + 1]);
  }
  
  std::string img2Name;
  {
    int argPos;
    if ((argPos = TOpt::findOpt(argc, argv, "--img2")) != -1)
      img2Name = std::string(argv[argPos + 1]);
  }
  
  int boundW = -1;
  TOpt::readNumericOpt(argc, argv, "--boundw", &boundW);
  int boundH = -1;
  TOpt::readNumericOpt(argc, argv, "--boundh", &boundH);
  
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
  
  TGraphic inGrp;
  TPngConversion::RGBAPngToGraphic(inFileName, inGrp);
  std::map<int, int> seenCelMap;
  for (int j = 0; j < imgCelH; j++) {
    for (int i = 0; i < imgCelW; i++) {
      int celNum = bgdIfs.readu16le();
      if ((i >= boundW) || (j >= boundH)) continue;
      // only change the first instance of each cel
      if (seenCelMap.find(celNum) != seenCelMap.end()) continue;
      seenCelMap[celNum] = celNum;
      
      const Cel& cel = cels.at(celNum);
      int srcX = i * celW;
      int srcY = j * celH;
      int dstX = cel.x;
      int dstY = cel.y;
      
      TGraphic* dstGrp = NULL;
      if (cel.unk5 == 0x9E) dstGrp = &img2Grp;
      else dstGrp = &img1Grp;
      
      dstGrp->copy(inGrp,
                TRect(dstX, dstY, celW, celH),
                TRect(srcX, srcY, celW, celH));
    }
  }
  
  if (!img1Name.empty())
    TPngConversion::graphicToRGBAPng(img1Name, img1Grp);
  if (!img2Name.empty())
    TPngConversion::graphicToRGBAPng(img2Name, img2Grp);
  
  return 0;
}
