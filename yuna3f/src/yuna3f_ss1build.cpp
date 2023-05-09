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
    std::cout << "Yuna 3 SS1 image builder" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <infile> <outfile>" << std::endl;
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outFile(argv[2]);
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(inFile, grp);
  
  PsxBitmap bitmap;
  bitmap.setFormat(PsxBitmap::format_rgb);
  bitmap.fromGraphic(grp, false);
  
  TBufStream dataIfs;
  bitmap.writePixelData(dataIfs);
  
  dataIfs.seek(0);
  TBufStream cmpOfs;
  Yuna3Cmp::cmpSs1(dataIfs, cmpOfs);
  
//  std::cerr << std::hex << cmpOfs.size() << std::endl;
  
  TBufStream ofs;
  if (cmpOfs.size() >= dataIfs.size()) {
    // uncompressed
    ofs.writeu32le(0x10);
    ofs.seek(0x10);
    ofs.writeu16le(grp.w());
    ofs.writeu16le(grp.h());
    
    dataIfs.seek(0);
    ofs.writeFrom(dataIfs, dataIfs.size());
  }
  else {
    // output compressed
    ofs.writeu16be(grp.w());
    ofs.writeu16be(grp.h());
    cmpOfs.seek(0);
    ofs.writeFrom(cmpOfs, cmpOfs.size());
  }
  
  ofs.save(outFile.c_str());
  
  return 0;
}
