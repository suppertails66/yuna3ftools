#include "dandy/DandyScriptOp.h"
#include "dandy/DandyScriptEnv.h"
#include "dandy/DandyScriptOpStereotypes.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
DandyScriptOp::DandyScriptOp()
  : stereotype(NULL),
    srcPos(-1) { }

DandyScriptOp::DandyScriptOp(int opcode)
  : stereotype(&DandyScriptOpStereotypes::matchStereotypeByOpcode(opcode)),
    srcPos(-1) { }

int DandyScriptOp::getSrcPos() const {
  return srcPos;
}

void DandyScriptOp::read(DandyScriptEnv& env, BlackT::TStream& ifs) {
  if (ifs.remaining() <= 1) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptOp::read()",
                            "Not enough space in stream to read opcode");
  }
  
  srcPos = ifs.tell();
  int opcode = ifs.readu16le();
  stereotype = &DandyScriptOpStereotypes::matchStereotypeByOpcode(opcode);
  
  if (stereotype->argString.compare("") == 0) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptOp::read()",
                            std::string("Unknown opcode: ")
                              + TStringConversion::intToString(opcode,
                                  TStringConversion::baseHex));
  }
  
  args.read(env, ifs, stereotype->argString);
  
/*  if (stereotype->opcode == 0x64) {
    std::cerr << std::hex << srcPos << std::endl;
    std::cerr << "  ";
    args.save(env, std::cerr);
    std::cerr << std::endl;
  } */
}

void DandyScriptOp::write(DandyScriptAsmEnv& env, BlackT::TStream& ofs) {
//  if (stereotype->opcode == 0) {
//    std::cerr << "start: " << ofs.tell() << std::endl;
//  }
  ofs.writeu16le(stereotype->opcode);
//  if (stereotype->opcode == 0) {
//    std::cerr << "end: " << ofs.tell() << std::endl;
//    char c;
//    std::cin >> c;
//  }
  args.write(env, ofs, stereotype->argString);
}

void DandyScriptOp::load(DandyScriptAsmEnv& env, BlackT::TStream& ifs) {
  std::string name = TParse::getSpacedSymbol(ifs);
//        std::cerr << name << std::endl;
  
//  const DandyScriptOpStereotype& stereotype
  stereotype = &DandyScriptOpStereotypes::matchStereotypeByName(name);
  args.load(env, ifs, stereotype->argString);
}

void DandyScriptOp::save(DandyScriptEnv& env, std::ostream& ofs) {
  // print op name
  ofs << stereotype->name;
  
  int nameLen = stereotype->name.size();
  int spacing = opArgSeparation - nameLen;
  for (int i = 0; i < spacing; i++) {
    ofs << " ";
  }
  ofs << " ";
  
//  ofs << "   ";
  
  // print arguments
  args.save(env, ofs, stereotype->argString);
  
  ofs << std::endl;
}


}
