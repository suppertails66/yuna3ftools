#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "util/TOpt.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
//using namespace Psx;

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
  if (argc < 4) {
    std::cout << "Yuna 3 archive builder" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <inprefix> <indexfile> <outfile> [flags]" << std::endl;
    std::cout << "Flags: " << std::endl;
    std::cout << "  --sectoralign   Pad files to sector boundaries"
      << std::endl;
    return 0;
  }
  
  std::string inPrefix(argv[1]);
  std::string indexFileName(argv[2]);
  std::string outFileName(argv[3]);
  
  bool padToSector = false;
  if (TOpt::hasFlag(argc, argv, "--sectoralign")) padToSector = true;
  
//  std::ifstream ifs((inPrefix + indexFileName).c_str());
  TBufStream ifs;
  ifs.open((inPrefix + indexFileName).c_str());
  
  TBufStream indexOfs;
  TBufStream dataOfs;
  
  int numFiles = TParse::matchInt(ifs);
  int oldIndexSize = TParse::matchInt(ifs);
  int newIndexSize = (numFiles * 0x10) + 0x10;
  
  indexOfs.writeu32be(numFiles);
  indexOfs.writeu32be(newIndexSize);
  indexOfs.writeu32be(0);
  indexOfs.writeu32be(0);
  
  for (int i = 0; i < numFiles; i++) {
    std::string filename = TParse::matchString(ifs);
    std::string physFileName = TParse::matchString(ifs);
    
    TBufStream fileIfs;
    if (!physFileName.empty()) {
      fileIfs.open((inPrefix + physFileName).c_str());
    }
    
//    std::cerr << filename << " " << physFileName << std::endl;
    
    int indexPos = indexOfs.tell();
    indexOfs.writeString(filename);
    while (indexOfs.tell() < (indexPos + 8)) indexOfs.put(0x00);
    
    if (!physFileName.empty())
      indexOfs.writeu32be(fileIfs.size());
    else
      indexOfs.writeu32be(0);
    
    indexOfs.writeu32be(dataOfs.tell() + newIndexSize);
    
    dataOfs.writeFrom(fileIfs, fileIfs.size());
    if (padToSector)
      while ((dataOfs.size() % 0x800) != 0) dataOfs.put(0x00);
  }
  
  TBufStream ofs;
  indexOfs.seek(0);
  dataOfs.seek(0);
  ofs.writeFrom(indexOfs, indexOfs.size());
  ofs.writeFrom(dataOfs, dataOfs.size());
  
  TFileManip::createDirectoryForFile(outFileName);
  ofs.save(outFileName.c_str());
  
  return 0;
}
