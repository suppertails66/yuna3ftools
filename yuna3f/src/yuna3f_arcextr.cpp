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
    std::cout << "Yuna 3 archive extractor" << std::endl;
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
  
  int numFiles = ifs.readu32be();
  int indexSize = ifs.readu32be();
  int unk1 = ifs.readu32be();
  int unk2 = ifs.readu32be();
  
  logOfs << numFiles << " " << indexSize << std::endl;
  
  for (int i = 0; i < numFiles; i++) {
    int basePos = ifs.tell();
    std::string filename;
    ifs.readCstrString(filename, 8);
    ifs.seek(basePos + 8);
    int size = ifs.readu32be();
    int offset = ifs.readu32be();
    
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
    
    ifs.seek(basePos + 0x10);
  }
  
  return 0;
}
