#ifndef DANDYSMALLPAC_H
#define DANDYSMALLPAC_H


#include "util/TStream.h"
#include <string>

namespace Psx {


class DandySmallPac {
public:
  
  static void unpack(BlackT::TStream& ifs, std::string outprefix);
  
protected:
  
};


}


#endif
