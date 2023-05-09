#ifndef DANDYSCRIPTOPSTEREOTYPE_H
#define DANDYSCRIPTOPSTEREOTYPE_H


#include "util/TStream.h"
#include <string>

namespace Psx {


class DandyScriptOpStereotype {
public:
  DandyScriptOpStereotype();
  DandyScriptOpStereotype(
    int opcode__, std::string name__, std::string argString__);
  DandyScriptOpStereotype(
    int opcode__, std::string name__, std::string aliases__,
    std::string argString__);
  
  int opcode;
  std::string name;
  std::string aliases;
  std::string argString;
protected:
  
};


}


#endif
