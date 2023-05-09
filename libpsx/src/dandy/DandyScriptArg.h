#ifndef DANDYSCRIPTARG_H
#define DANDYSCRIPTARG_H


#include "dandy/DandyScriptEnv.h"
#include "dandy/DandyScriptAsmEnv.h"
#include "util/TStream.h"
#include <string>
#include <iostream>

namespace Psx {


class DandyScriptArg {
public:
  friend class DandyScriptArgList;

  enum ArgType {
    type_none,
    type_int8,
    type_int16,
    type_int24,
    type_int32,
    type_cstring,
    type_gameTextString,
    type_labelIndexReference
  };
  
  DandyScriptArg();
  
  void print(DandyScriptEnv& env, std::ostream& ofs);
  void write(DandyScriptAsmEnv& env, BlackT::TStream& ofs);
protected:
  ArgType type_;
  int intValue_;
  std::string stringValue_;
};


}


#endif
