#include "pom/PomPlmStringScanException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include <iostream>

using namespace BlackT;

namespace Psx {


PomPlmStringScanException::PomPlmStringScanException(
               const char* nameOfSourceFile__,
               int lineNum__,
               const std::string& source__,
               std::string problem__)
  : TException(nameOfSourceFile__,
               lineNum__,
               source__),
    problem_(problem__) { }


}
