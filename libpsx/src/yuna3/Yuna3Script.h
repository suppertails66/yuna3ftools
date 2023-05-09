#ifndef YUNA3SCRIPT_H
#define YUNA3SCRIPT_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TThingyTable.h"
#include "yuna3/Yuna3ScriptOp.h"
#include <map>
#include <string>
#include <vector>

namespace Psx {


typedef std::vector<Yuna3ScriptOp> Yuna3ScriptOpCollection;

class Yuna3Script {
public:
  
  Yuna3Script();
  
  void read(BlackT::TStream& ifs, int size);
  
  Yuna3ScriptOpCollection ops;
  
protected:
  
};


}


#endif
