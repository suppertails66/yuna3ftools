#ifndef YUNA3SCRIPTOP_H
#define YUNA3SCRIPTOP_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TThingyTable.h"
#include <map>
#include <string>
#include <vector>

namespace Psx {


class Yuna3ScriptOp {
public:
  
  Yuna3ScriptOp();
  
//;   - 1b opcode
//;   - 1b (high nybble only) param count (in words?)
//;   - 1b not used?
//;   - 1b stack arg bitfield
//;        - example: if bit 7 is set, then param 1 is not used as written
//;          (at least as long as the standard arg read routines are used).
//;          instead, it is read from the script stack (based at $2718)
//;          using the low byte of the nominal parameter 1 value as an offset
//;          from the current(?) position.
//;   - parameters
  BlackT::TByte opcode;
  //int paramCount;
  BlackT::TByte unknown;
  BlackT::TByte stackArgFlags;
  std::vector<int> params;
  
  int offset;
  
  void read(BlackT::TStream& ifs);
  
protected:
  
};


}


#endif
