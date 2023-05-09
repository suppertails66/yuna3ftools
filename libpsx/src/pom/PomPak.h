#ifndef POMPAK_H
#define POMPAK_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include <string>
#include <vector>

namespace Psx {


class PomPak {
public:
  typedef BlackT::TArray<BlackT::TByte> DataArray;
  typedef std::vector< BlackT::TArray<BlackT::TByte> > DataArrayCollection;

  PomPak();
  
  void read(BlackT::TStream& ifs, int inputSize = -1);
  void write(BlackT::TStream& ofs) const;
  void save(std::string outPrefix) const;
  void load(std::string inPrefix);
  
  void decmpRux();
  void cmpRux();
  
  DataArrayCollection data;
  
protected:
//  const static int sectorSize = 0x800;
  
  static void readDataArray(BlackT::TStream& ifs,
                     int chunkOffset, int chunkSize,
                     DataArray& dst);
  
//  static void readDataArraySectors(BlackT::TStream& ifs,
//                     int chunkSecOffset, int chunkSecSize,
//                     DataArray& dst);
  
};


}


#endif
