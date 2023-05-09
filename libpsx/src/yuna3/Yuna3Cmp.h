#ifndef YUNA3CMP_H
#define YUNA3CMP_H


#include "util/TStream.h"
#include <string>
#include <list>
#include <map>
#include <unordered_map>

namespace Psx {


class Yuna3Cmp {
public:
  
  // decompress ss1 image.
  // data is assumed to be advanced past the 4b image dimensions.
  static void decmpSs1(BlackT::TStream& ifs, BlackT::TStream& ofs,
                       int outputSize = -1);
  
  // compress ss1 data.
  // does not output dimension data.
  static void cmpSs1(BlackT::TStream& ifs, BlackT::TStream& ofs);
  
protected:
  
  static int countNextWordRepeats(BlackT::TStream& ifs);
  static int countWordRepeats(BlackT::TStream& ifs, int value);
  
};


}


#endif
