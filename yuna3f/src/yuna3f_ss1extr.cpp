#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSoundFile.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxBitmap.h"
#include "yuna3/Yuna3Cmp.h"
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
  if (argc < 3) {
    std::cout << "Yuna 3 SS1 image extractor" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <infile> <outfile>" << std::endl;
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outFile(argv[2]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  int first = ifs.readu32le();
  
  ifs.seek(0);
  int w = ifs.readu16be();
  int h = ifs.readu16be();
  
  // if first word of file is 0x10 (header size?), content is uncompressed
  // this may seem like a hack, but it's the exact check the game's own
  // decompression code does
  bool uncompressed = false;
  if (first == 0x10) {
    uncompressed = true;
    ifs.seek(0x10);
    w = ifs.readu16le();
    h = ifs.readu16le();
  }
  
  int outputSize = (w * h * 2);
  
  TBufStream ofs;
  
  if (uncompressed) {
    for (int i = 0; i < outputSize / 2; i++) {
      int next = ifs.readu16be();
      ofs.writeu16be(next);
    }
  }
  else {
    Yuna3Cmp::decmpSs1(ifs, ofs, outputSize);
  }
  
//  std::cerr << std::hex << ofs.size() << std::endl;
  
  PsxBitmap bitmap;
  ofs.seek(0);
  bitmap.readPixelData(ofs, PsxBitmap::format_rgb, w, h);
//  bitmap.readPixelData(ofs, PsxBitmap::format_8bit, w, h);
  
  TGraphic grp;
  bitmap.toGraphic(grp, true, false);
//  bitmap.toGrayscaleGraphic(grp);
  
  TPngConversion::graphicToRGBAPng(outFile, grp);
  
  return 0;
}
