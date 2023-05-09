#include "dandy/DandyScriptOpStereotype.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
DandyScriptOpStereotype::DandyScriptOpStereotype()
  : opcode(-1) { }

DandyScriptOpStereotype::DandyScriptOpStereotype(
    int opcode__, std::string name__, std::string argString__)
  : opcode(opcode__),
    name(name__),
    aliases(""),
    argString(argString__) { }

DandyScriptOpStereotype::DandyScriptOpStereotype(
    int opcode__, std::string name__, std::string aliases__,
    std::string argString__)
  : opcode(opcode__),
    name(name__),
    aliases(aliases__),
    argString(argString__) { }


}
