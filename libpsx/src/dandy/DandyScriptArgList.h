#ifndef DANDYSCRIPTARGLIST_H
#define DANDYSCRIPTARGLIST_H


#include "dandy/DandyScriptArg.h"
#include "dandy/DandyScriptEnv.h"
#include "dandy/DandyScriptAsmEnv.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <iostream>

namespace Psx {


class DandyScriptArgList {
public:
  DandyScriptArgList();
  
  void read(DandyScriptEnv& env,
            BlackT::TStream& ifs, std::string argString);
  void write(DandyScriptAsmEnv& env,
             BlackT::TStream& ofs, std::string argString);
  void save(DandyScriptEnv& env,
            std::ostream& ofs, std::string argString);
  void load(DandyScriptAsmEnv& env,
            BlackT::TStream& ifs, std::string argString);
protected:
  typedef std::vector<DandyScriptArg> ScriptArgCollection;
  
  void addIntArg(int value, int size);
  
  struct SpecialRefArg {
    std::string type;
    std::string name;
    
    void match(BlackT::TStream& ifs);
  };
  
  ScriptArgCollection args_;
};


}


#endif
