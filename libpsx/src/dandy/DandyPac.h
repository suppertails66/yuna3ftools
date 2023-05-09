#ifndef DANDYPAC_H
#define DANDYPAC_H


#include "util/TStream.h"
#include <string>

namespace Psx {


class DandyPac {
public:
  
  static void pack(std::string inprefix, int filecount, BlackT::TStream& ofs);
  static void unpack(BlackT::TStream& ifs, std::string outprefix);
  
protected:
  const static int sectorSize = 0x800;
  const static int indexEntrySize = 8;
  
};


}


#endif
