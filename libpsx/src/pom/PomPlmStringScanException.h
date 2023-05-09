#ifndef POMPLMSTRINGSCANEXCEPTION_H
#define POMPLMSTRINGSCANEXCEPTION_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "exception/TException.h"
#include <string>
#include <vector>

namespace Psx {


class PomPlmStringScanException : public BlackT::TException {
public:
  PomPlmStringScanException(const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__,
                 std::string problem__);
protected:
  std::string problem_;
};


}


#endif
