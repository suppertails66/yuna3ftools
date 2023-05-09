#include "dandy/DandyScriptArg.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
DandyScriptArg::DandyScriptArg()
  : type_(type_none),
    intValue_(-1) { }

void DandyScriptArg::print(DandyScriptEnv& env, std::ostream& ofs) {
  switch (type_) {
  case type_int8:
  case type_int16:
  case type_int24:
  case type_int32:
    ofs << TStringConversion::intToString(intValue_,
              TStringConversion::baseHex);
    break;
  case type_cstring:
    ofs << "\"" << stringValue_ << "\"";
    break;
  case type_gameTextString:
    ofs << "<TEXT_"
      << TStringConversion::intToString(intValue_ + env.basePos,
            TStringConversion::baseHex)
      << ">";
    break;
  case type_labelIndexReference:
    ofs << "&\"" << stringValue_ << "\"";
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptArg::print()",
                            std::string("Tried to print unknown type: ")
                              + TStringConversion::intToString(type_));
    break;
  }
}

void DandyScriptArg::write(DandyScriptAsmEnv& env, BlackT::TStream& ofs) {
  switch (type_) {
  case type_int8:
    ofs.writeu8(intValue_);
    break;
  case type_int16:
    ofs.writeu16le(intValue_);
    break;
//  case type_int24:
//    ofs.writeu24le(intValue_);
//    break;
  case type_int32:
    ofs.writeu32le(intValue_);
    break;
  case type_cstring:
    ofs.writeString(stringValue_);
    ofs.put(0x00);
    break;
  case type_gameTextString:
  {
    std::string newString;
    
    // remapped string
    if (intValue_ != -1) {
      int origStrOffset = intValue_;
      newString = env.strings.at(origStrOffset).str;
    }
    // common string
    else {
      std::string strName = stringValue_;
      newString = env.commonStrings.at(strName).str;
    }
    
    ofs.writeString(newString);
    ofs.put(0x00);
  }
    break;
  case type_labelIndexReference:
    {
    // add reference to environment
    DandyScriptReference reference;
    reference.offset = ofs.tell();
    reference.type = DandyScriptReference::type_8bit;
    reference.name = stringValue_;
    env.addReference(reference);
    
    // placeholder for resolved reference
    ofs.put(0x00);
    }
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptArg::write()",
                            std::string("Tried to write unknown type: ")
                              + TStringConversion::intToString(type_));
    break;
  }
}


}
