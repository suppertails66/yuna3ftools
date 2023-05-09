#ifndef DANDYSCRIPTDISASSEMBLER_H
#define DANDYSCRIPTDISASSEMBLER_H


#include "dandy/DandyScriptEnv.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <iostream>

namespace Psx {


class DandyScriptDisassembler {
public:
  DandyScriptDisassembler(DandyScriptEnv& env__,
                          BlackT::TStream& ifs__,
                          std::ostream& ofs__);
  
  void operator()();
protected:
  DandyScriptEnv& env;
  BlackT::TStream& ifs;
  std::ostream& ofs;
};


}


#endif
