#include "pom/PomMapData.h"
#include "pom/PomCmp.h"
#include "pom/PomPak.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {


PomMapData::PomMapData() { }

void PomMapData::read(BlackT::TStream& ifs, bool decompress) {
  int basePos = ifs.tell();
  int baseSec = basePos / sectorSize;
  
  readDataArraySectors(ifs,
    baseSec + pbavSecOffset, pbavSecSize, pbavChunk);
  readDataArraySectors(ifs,
    baseSec + chunk1SecOffset, chunk1SecSize, chunk1);
  readDataArraySectors(ifs,
    baseSec + chunk3SecOffset, chunk3SecSize, chunk3);
  
  {
    PomPak pak;
    ifs.seek(basePos + (resourcesSecOffset * sectorSize));
    pak.read(ifs, resourcesSecSize * sectorSize);
    if (decompress) pak.decmpRux();
    resources = pak.data;
  }
  
  {
    PomPak pak;
    ifs.seek(basePos + (pqesSecOffset * sectorSize));
    pak.read(ifs, pqesSecSize * sectorSize);
    pqesChunks = pak.data;
  }
}

void PomMapData::write(BlackT::TStream& ofs, bool compress) const {
  writeDataArraySectors(pbavChunk, pbavSecSize, ofs);
  writeDataArraySectors(chunk1, chunk1SecSize, ofs);
  writeDataArraySectors(chunk3, chunk3SecSize, ofs);
  
  {
    PomPak pak;
    pak.data = resources;
    if (compress) pak.cmpRux();
//    pak.write(ofs);

    // issues with automatically determining the size of the
    // resource chunk, combined with the fact that we are no longer
    // de- and re-compressing all files, causes an issue where
    // the last file in the resource chunk may have a huge chunk of
    // filler attached.
    // we don't want that to overwrite the pqes chunk, so make sure
    // not to write more data than will fit.
    int targetSize = resourcesSecSize * sectorSize;
    TBufStream tmp;
    pak.write(tmp);
    ofs.write(tmp.data().data(),
              std::min(tmp.size(), targetSize));
    
    ofs.padToSize((resourcesSecOffset + resourcesSecSize) * sectorSize,
                  0x00);
  }
  
  {
    PomPak pak;
    pak.data = pqesChunks;
//    pak.write(ofs);

    // as above
    int targetSize = pqesSecSize * sectorSize;
    TBufStream tmp;
    pak.write(tmp);
    ofs.write(tmp.data().data(),
              std::min(tmp.size(), targetSize));

    ofs.padToSize((pqesSecOffset + pqesSecSize) * sectorSize,
                  0x00);
  }
}

void PomMapData::save(std::string outPrefix) const {
  TFileManip::createDirectoryForFile((outPrefix + "anything.bin").c_str());
  
  saveDataArrayFile(pbavChunk, outPrefix + "pbav.bin");
  saveDataArrayFile(chunk1, outPrefix + "chunk1.bin");
  saveDataArrayFile(chunk3, outPrefix + "chunk3.bin");
  
  {
    PomPak pak;
    pak.data = resources;
    pak.save(outPrefix + "resources/");
  }
  
  {
    PomPak pak;
    pak.data = pqesChunks;
    pak.save(outPrefix + "pqes/");
  }
}

void PomMapData::load(std::string inPrefix) {
  loadDataArrayFile(inPrefix + "pbav.bin", pbavChunk);
  loadDataArrayFile(inPrefix + "chunk1.bin", chunk1);
  loadDataArrayFile(inPrefix + "chunk3.bin", chunk3);
  
  {
    PomPak pak;
    pak.load(inPrefix + "resources/");
    resources = pak.data;
  }
  
  {
    PomPak pak;
    pak.load(inPrefix + "pqes/");
    pqesChunks = pak.data;
  }
}

void PomMapData::readDataArray(BlackT::TStream& ifs,
                   int chunkOffset, int chunkSize,
                   DataArray& dst) {
  ifs.seek(chunkOffset);
  dst.resize(chunkSize);
  for (int i = 0; i < chunkSize; i++) {
    dst[i] = ifs.get();
  }
}

void PomMapData::readDataArraySectors(BlackT::TStream& ifs,
                   int chunkSecOffset, int chunkSecSize,
                   DataArray& dst) {
  readDataArray(ifs,
    chunkSecOffset * sectorSize, chunkSecSize * sectorSize,
    dst);
}

void PomMapData::writeDataArray(const DataArray& src,
                   int chunkSize,
                   BlackT::TStream& ofs) {
  int padding = chunkSize - src.size();
  if (padding < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "PomMapData::writeDataArray()",
                            "Input data size exceeds chunk size limit");
  }
  
  for (int i = 0; i < src.size(); i++) ofs.put(src[i]);
  for (int i = 0; i < padding; i++) ofs.put(0x00);
}

void PomMapData::writeDataArraySectors(const DataArray& src,
                   int chunkSecSize,
                   BlackT::TStream& ofs) {
  writeDataArray(src,
    chunkSecSize * sectorSize,
    ofs);
}

void PomMapData::saveDataArray(
    const DataArray& src, BlackT::TStream& ofs) {
  ofs.write((const char*)src.data(), src.size());
}

void PomMapData::saveDataArrayFile(
    const DataArray& src, std::string dstName) {
  TBufStream ofs;
  saveDataArray(src, ofs);
  ofs.save(dstName.c_str());
}

void PomMapData::loadDataArray(BlackT::TStream& ifs, DataArray& dst) {
  dst.resize(ifs.remaining());
  for (int i = 0; i < dst.size(); i++) dst[i] = ifs.get();
}

void PomMapData::loadDataArrayFile(std::string srcName, DataArray& dst) {
  TBufStream ifs;
  ifs.open(srcName.c_str());
  loadDataArray(ifs, dst);
}


}
