#include "dandy/DandyScriptEnv.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
DandyScriptEnv::DandyScriptEnv()
  : basePos(0) { }

void DandyScriptEnv::addLabel(DandyScriptLabel label) {
  labels.push_back(label);
}

/*void DandyScriptEnv::disassemble(BlackT::TStream& ifs, std::ostream& ofs) {
//  read(ifs);
//  save(ofs);
}

void DandyScriptEnv::assemble(std::istream& ifs, BlackT::TStream& ofs) {
//  load(ifs);
//  write(ofs);
}

// convert op data to text script
void DandyScriptEnv::save(std::ostream& ofs) {
  
}

// convert text script to op data
void DandyScriptEnv::load(std::istream& ifs) {
  
}

// convert binary to op data
void DandyScriptEnv::read(BlackT::TStream& ifs) {
  
}

// convert op data to binary
void DandyScriptEnv::write(BlackT::TStream& ofs) {
  
} */


}
