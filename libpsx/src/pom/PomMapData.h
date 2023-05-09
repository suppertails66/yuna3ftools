#ifndef POMMAPDATA_H
#define POMMAPDATA_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include <string>
#include <vector>

namespace Psx {


class PomMapData {
public:
  typedef BlackT::TArray<BlackT::TByte> DataArray;
  typedef std::vector< BlackT::TArray<BlackT::TByte> > DataArrayCollection;

  PomMapData();
  
  void read(BlackT::TStream& ifs, bool decompress = true);
  void write(BlackT::TStream& ofs, bool compress = true) const;
  void save(std::string outPrefix) const;
  void load(std::string inPrefix);
  
  DataArray pbavChunk;
  DataArray chunk1;
  DataArray chunk3;
  DataArrayCollection resources;
  DataArrayCollection pqesChunks;
//  BlackT::TBufStream pbav;
  
protected:
  const static int sectorSize = 0x800;
  
  const static int pbavSecOffset = 0x0;
  const static int pbavSecSize = 0x7 - 0x0;
  const static int chunk1SecOffset = 0x7;
//  const static int chunk1SecSize = 0x046 - 0x007;
  const static int chunk1SecSize = 0x04D - 0x007;
  const static int chunk3SecOffset = 0x04D;
  const static int chunk3SecSize = 0x093 - 0x04D;
  const static int resourcesSecOffset = 0x093;
  const static int resourcesSecSize = 0x0DB - 0x093;
  const static int pqesSecOffset = 0x0DB;
  const static int pqesSecSize = 0x0E5 - 0x0DB;
//  const static int extraDataSecOffset = 0xEB;
//  const static int extraDataSecSize =  - 0xEB;
  
  static void readDataArray(BlackT::TStream& ifs,
                     int chunkOffset, int chunkSize,
                     DataArray& dst);
  static void readDataArraySectors(BlackT::TStream& ifs,
                     int chunkSecOffset, int chunkSecSize,
                     DataArray& dst);
  
  static void writeDataArray(const DataArray& src,
                     int chunkSize,
                     BlackT::TStream& ofs);
  static void writeDataArraySectors(const DataArray& src,
                     int chunkSecSize,
                     BlackT::TStream& ofs);
  
  static void saveDataArray(const DataArray& src, BlackT::TStream& ofs);
  static void saveDataArrayFile(const DataArray& src, std::string dstName);
  
  static void loadDataArray(BlackT::TStream& ifs, DataArray& dst);
  static void loadDataArrayFile(std::string srcName, DataArray& dst);
  
};


}


#endif
