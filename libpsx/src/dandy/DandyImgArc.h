#ifndef DANDYIMGARC_H
#define DANDYIMGARC_H


#include "util/TStream.h"
#include <string>

namespace Psx {


class DandyImgArc {
public:
  
//  static void pack(std::string inprefix, int filecount, BlackT::TStream& ofs);
  static void unpack(BlackT::TStream& ifs, std::string outprefix);
  static void unpackDecompressed(BlackT::TStream& ifs, std::string outprefix);
  static void pack(BlackT::TStream& ofs, int numFiles,
                   std::string inprefix);
  static void packDecompressed(BlackT::TStream& ofs, int numFiles,
                   std::string inprefix);
  
protected:
//  const static int sectorSize = 0x800;
//  const static int indexEntrySize = 8;
  
};


}


#endif
