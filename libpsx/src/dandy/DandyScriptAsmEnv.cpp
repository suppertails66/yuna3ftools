#include "dandy/DandyScriptAsmEnv.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
DandyScriptAsmEnv::DandyScriptAsmEnv()
   { }

void DandyScriptAsmEnv::addLabel(DandyScriptLabel label) {
  labels.push_back(label);
}

void DandyScriptAsmEnv::addReference(DandyScriptReference reference) {
  references.push_back(reference);
}

void DandyScriptAsmEnv::writeLabelBlock(BlackT::TStream& ofs) const {
  ofs.writeu32le(labels.size());
  for (LabelCollection::const_iterator it = labels.cbegin();
       it != labels.cend();
       ++it) {
    const DandyScriptLabel& label = *it;
    
    int paddingSize = 16 - label.name.size();
    
    if (paddingSize < 0) {
      throw TGenericException(T_SRCANDLINE,
                              "DandyScriptAsmEnv::writeLabelBlock()",
                              std::string("Label name too long: ")
                                + label.name);
    }
    
    ofs.writeString(label.name);
    for (int i = 0; i < paddingSize; i++) ofs.put(0x00);
    ofs.writeu32le(label.value);
  }
}


}
