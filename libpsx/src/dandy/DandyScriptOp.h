#ifndef DANDYSCRIPTOP_H
#define DANDYSCRIPTOP_H


#include "dandy/DandyScriptOpStereotypes.h"
#include "dandy/DandyScriptArgList.h"
#include "dandy/DandyScriptAsmEnv.h"
#include "util/TStream.h"
#include <string>
#include <iostream>

namespace Psx {

class DandyScriptEnv;

class DandyScriptOp {
public:
  DandyScriptOp();
  DandyScriptOp(int opcode);
  
  int getSrcPos() const;
  
  void read(DandyScriptEnv& env, BlackT::TStream& ifs);
  void write(DandyScriptAsmEnv& env, BlackT::TStream& ofs);
  void load(DandyScriptAsmEnv& env, BlackT::TStream& ifs);
  void save(DandyScriptEnv& env, std::ostream& ofs);
protected:
  const static int opArgSeparation = 16;

  const DandyScriptOpStereotype* stereotype;
  DandyScriptArgList args;
  int srcPos;
};


}


#endif
