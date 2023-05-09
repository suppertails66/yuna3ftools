#include "pom/PomPak.h"
#include "pom/PomCmp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {


PomPak::PomPak() { }

void PomPak::read(BlackT::TStream& ifs, int inputSize) {
  if (inputSize == -1) inputSize = ifs.size();  
  
  int basePos = ifs.tell();  
  
  // detect size
  int numFiles = ifs.readu32le() / 4;
  
  data.resize(numFiles);
  for (int i = 0; i < numFiles; i++) {
    DataArray& dst = data[i];
    
    ifs.seek(basePos + (i * 4));
    int offset = ifs.readu32le();
    int fileSize;
    if (i != (numFiles - 1)) {
      fileSize = ifs.readu32le() - offset;
    }
    else {
      fileSize = inputSize - offset;
    }
    
//    dst.resize(fileSize);
//    ifs.seek(offset);
//    for (int i = 0; i < fileSize; i++) dst[i] = ifs.get();
    readDataArray(ifs, basePos + offset, fileSize, dst);
  }
}

void PomPak::write(BlackT::TStream& ofs) const {
  int basePos = ofs.tell();
  int numFiles = data.size();
  
  // reserve space for index
  ofs.seekoff(numFiles * 4);
  
  for (unsigned int i = 0; i < data.size(); i++) {
    int outPos = ofs.tell();
    ofs.seek(basePos + (i * 4));
    ofs.writeu32le(outPos - basePos);
    
    ofs.seek(outPos);
    ofs.write((char*)data[i].data(), data[i].size());
  }
}

void PomPak::save(std::string outPrefix) const {
  TFileManip::createDirectoryForFile((outPrefix + "anything.bin").c_str());
  
  for (unsigned int i = 0; i < data.size(); i++) {
    TBufStream ofs;
    ofs.write((const char*)data[i].data(), data[i].size());
    
    std::string outname = outPrefix
//      + "-"
      + TStringConversion::intToString(i)
      + ".bin";
    ofs.save(outname.c_str());
  }
}

void PomPak::load(std::string inPrefix) {
  int fileNum = -1;
  while (true) {
    ++fileNum;
    std::string filename = inPrefix
      + TStringConversion::intToString(fileNum)
      + ".bin";
    if (!TFileManip::fileExists(filename.c_str())) break;
    
    TBufStream ifs;
    ifs.open(filename.c_str());
    
    DataArray subdata;
    subdata.resize(ifs.size());
    for (int i = 0; i < subdata.size(); i++) subdata[i] = ifs.get();
    data.push_back(subdata);
  }
}

void PomPak::decmpRux() {
  for (DataArrayCollection::iterator it = data.begin();
       it != data.cend();
       ++it) {
    DataArray& dataArray = *it;
    
    TBufStream ifs;
    ifs.write((char*)dataArray.data(), dataArray.size());
    ifs.seek(0);
    
    TBufStream ofs;
    PomCmp::decmpRux(ifs, ofs);
    ofs.seek(0);
    dataArray.resize(ofs.size());
    for (unsigned int i = 0; i < dataArray.size(); i++)
      dataArray[i] = ofs.get();
  }
}

void PomPak::cmpRux() {
  for (DataArrayCollection::iterator it = data.begin();
       it != data.cend();
       ++it) {
    DataArray& dataArray = *it;
    
    TBufStream ifs;
    ifs.write((char*)dataArray.data(), dataArray.size());
    ifs.seek(0);
    
    TBufStream ofs;
    PomCmp::cmpRux(ifs, ofs);
    ofs.seek(0);
    dataArray.resize(ofs.size());
    for (unsigned int i = 0; i < dataArray.size(); i++)
      dataArray[i] = ofs.get();
  }
}

void PomPak::readDataArray(BlackT::TStream& ifs,
                   int chunkOffset, int chunkSize,
                   DataArray& dst) {
  ifs.seek(chunkOffset);
  dst.resize(chunkSize);
  for (int i = 0; i < chunkSize; i++) {
    dst[i] = ifs.get();
  }
}


}
