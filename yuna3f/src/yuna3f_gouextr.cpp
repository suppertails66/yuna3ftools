#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
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
    std::cout << "Yuna 3 GOU archive extractor" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <infile> <outprefix> <outindexfile>" << std::endl;
    return 0;
  }
  
  std::string inFile(argv[1]);
  std::string outPrefix(argv[2]);
  std::string logFile(argv[3]);
  
  TFileManip::createDirectoryForFile(logFile);
  std::ofstream logOfs(logFile.c_str());
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  std::string filename;
  ifs.readCstrString(filename, 12);
  ifs.seek(12);
  int numFiles = ifs.readu32le();
  int indexOffset = ifs.readu32le();
  int indexSizeWithPadding = ifs.readu32le();
  int indexSize = ifs.readu32le();
  int indexUnk1 = ifs.readu32le();
  
  logOfs << "\"" << filename << "\""
    << " " << numFiles
    << " " << indexOffset
    << " " << indexSizeWithPadding
    << " " << indexSize
    << " " << indexUnk1
    << std::endl;
  
  for (int i = 0; i < numFiles; i++) {
    int basePos = ifs.tell();
    
    std::string filename;
    ifs.readCstrString(filename, 16);
    ifs.seek(basePos + 16);
    int offset = ifs.readu32le();
    int sizeWithPadding = ifs.readu32le();
    int size = ifs.readu32le();
    int unk1 = ifs.readu32le();
    
    std::cout << filename << " " << std::hex << offset << " " << size
      << std::endl;
    
    TBufStream ofs;
    std::string outName;
    std::string outLogName;
    if (size != 0) {
//      outName = outPrefix + filename;
      outLogName = filename;
      outName = outPrefix + filename;
      ifs.seek(offset);
      ofs.writeFrom(ifs, size);
      TFileManip::createDirectoryForFile(outName.c_str());
      ofs.save(outName.c_str());
    }
    
    logOfs << "\"" << filename << "\""
      << "\"" << outLogName << "\"" << endl;
    
    ifs.seek(basePos + 0x20);
  }
  
  return 0;
}
