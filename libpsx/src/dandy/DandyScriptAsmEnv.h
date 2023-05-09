#ifndef DANDYSCRIPTASMENV_H
#define DANDYSCRIPTASMENV_H


//#include "dandy/DandyScriptOp.h"
#include "dandy/DandyScriptLabel.h"
#include "dandy/DandyScriptReference.h"
#include "dandy/DandyScriptReader.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace Psx {


class DandyScriptAsmEnv {
public:
  typedef std::vector<DandyScriptLabel> LabelCollection;
  typedef std::vector<DandyScriptReference> ReferenceCollection;
  typedef std::map<int, DandyScriptReader::ResultString> OffsetToStringMap;
  typedef std::map<std::string, DandyScriptReader::ResultString>
    NameToStringMap;
  
  DandyScriptAsmEnv();
  
  void addLabel(DandyScriptLabel label);
  void addReference(DandyScriptReference reference);
  
  void writeLabelBlock(BlackT::TStream& ofs) const;
  
//  int basePos;
  LabelCollection labels;
  ReferenceCollection references;
  OffsetToStringMap strings;
  NameToStringMap commonStrings;
protected:
  
};


}


#endif
