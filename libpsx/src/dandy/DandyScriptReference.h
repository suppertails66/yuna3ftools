#ifndef DANDYSCRIPTREFERENCE_H
#define DANDYSCRIPTREFERENCE_H


#include <string>

namespace Psx {


struct DandyScriptReference {
  enum Type {
    type_8bit
  };
  
  int offset;
  std::string name;
  Type type;
};


}


#endif
