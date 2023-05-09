#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSoundFile.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxXa.h"
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
  if (argc < 2) {
    std::cout << "Yuna 3 XA extractor" << std::endl;
    std::cout << "Usage: " << argv[0]
      << " <outprefix>" << std::endl;
    return 0;
  }
  
  std::string outPrefix(argv[1]);
  
  TBufStream ifs;
  ifs.open("disc/files/DATA/ACMFILE.DAT");
  
  int numFiles = ifs.size() / 16;
//  int numFiles = 32;
  
  TBufStream voiceIfs;
  int activeFileNum = -1;
  for (int i = 0; i < numFiles; i++) {
    int basePos = ifs.tell();
    std::string filename;
    ifs.readCstrString(filename, 16);
    ifs.seek(basePos + 16);
    
    int fileNum = i / 32;
    int channelNum = i % 32;
    
    // switch files if needed
    if (activeFileNum != fileNum) {
      std::string targetFileName
        = std::string("disc/rawfiles/VOICE/VOICE")
          + TStringConversion::intToString(fileNum + 1,
              TStringConversion::baseDec)
          + ".STR";
      std::cout << "opening: " << targetFileName << std::endl;
      voiceIfs.open(targetFileName.c_str());
      activeFileNum = fileNum;
    }
    
    std::cout << "  " << filename << std::endl;
    
    TSoundFile dst;
    voiceIfs.seek(channelNum * 0x930);
    PsxXa::decodeXa(voiceIfs, dst);
    
    TBufStream ofs;
    dst.exportWav(ofs);
    
    // assume .ACM extension
    std::string outFileName = outPrefix
      + filename.substr(0, filename.size() - 4)
      + ".wav";
    TFileManip::createDirectoryForFile(outFileName.c_str());
    ofs.save(outFileName.c_str());
  }
  
  return 0;
}
