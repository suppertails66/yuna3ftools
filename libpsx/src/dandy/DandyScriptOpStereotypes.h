#ifndef DANDYSCRIPTOPSTEREOTYPES_H
#define DANDYSCRIPTOPSTEREOTYPES_H


#include "dandy/DandyScriptOpStereotype.h"
#include "util/TStream.h"
#include <string>

namespace Psx {


class DandyScriptOpStereotypes {
public:
  const static DandyScriptOpStereotype& matchStereotypeByName(std::string name);
  const static DandyScriptOpStereotype& matchStereotypeByOpcode(int opcode);
protected:
  
};


}


#endif
