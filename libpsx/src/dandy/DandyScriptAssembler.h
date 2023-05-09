#ifndef DANDYSCRIPTASSEMBLER_H
#define DANDYSCRIPTASSEMBLER_H


#include "dandy/DandyScriptAsmEnv.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <iostream>

namespace Psx {


class DandyScriptAssembler {
public:
  DandyScriptAssembler(DandyScriptAsmEnv& env__,
//                       std::istream& src__,
                       BlackT::TStream& src__,
                       BlackT::TStream& ofs__);
  
  void operator()();
protected:
  DandyScriptAsmEnv& env;
//  std::istream& src;
  BlackT::TStream& src;
  BlackT::TStream& ofs;
  bool isInclude;
  std::string includedFileName;
};


}


#endif
