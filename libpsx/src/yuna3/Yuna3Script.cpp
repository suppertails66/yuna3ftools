#include "yuna3/Yuna3Script.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <cctype>
#include <iostream>
#include <fstream>

using namespace BlackT;

namespace Psx {


Yuna3Script::Yuna3Script()
  { }

void Yuna3Script::read(BlackT::TStream& ifs, int size) {
  ops.clear();  
  
  int startOffset = ifs.tell();
  int endOffset = ifs.tell() + size;
  while (ifs.tell() < endOffset) {
    int offset = ifs.tell() - startOffset;
    
    Yuna3ScriptOp op;
    op.read(ifs);
    op.offset = offset;
    ops.push_back(op);
  }
}


}
