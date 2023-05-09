#ifndef GRPACCMP_H
#define GRPACCMP_H


#include "util/TStream.h"

namespace Psx {


class GrPacCmp {
public:
  static int decmpLz(BlackT::TStream& src, BlackT::TStream& dst,
              int remaining);
  
protected:
  const static int maxRunLength = 256;

  static inline int fetchBit(BlackT::TStream& src, int& command, int& counter,
      int& remaining);
  
  static inline void loadCommandBits(
      BlackT::TStream& src, int& command, int& counter,
      int& remaining);
  
  static inline void lookbackCopy(BlackT::TStream& dst,
                                  int distance, int runlen);
  
};


}


#endif
