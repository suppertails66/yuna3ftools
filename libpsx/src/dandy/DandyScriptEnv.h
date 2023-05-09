#ifndef DANDYSCRIPTENV_H
#define DANDYSCRIPTENV_H


//#include "dandy/DandyScriptOp.h"
#include "dandy/DandyScriptLabel.h"
#include "dandy/DandyScriptReference.h"
#include "util/TStream.h"
#include <string>
#include <vector>
#include <iostream>

namespace Psx {


//struct DandyScriptReference {
//  int offset;
//  std::string name;
//};

class DandyScriptEnv {
public:
  typedef std::vector<DandyScriptLabel> LabelCollection;
//  typedef std::vector<DandyScriptReference> ReferenceCollection;
  
  DandyScriptEnv();
  
  void addLabel(DandyScriptLabel label);
//  void addReference(DandyScriptReference reference);
  
  int basePos;
  LabelCollection labels;
//  ReferenceCollection references;
  
//  void disassemble(BlackT::TStream& ifs, std::ostream& ofs);
//  void assemble(std::istream& ifs, BlackT::TStream& ofs);
protected:
//  typedef std::vector<DandyScriptOp> OpCollection;
//  OpCollection ops;
  
  // convert op data to text script
//  void save(std::ostream& ofs);
  // convert text script to op data
//  void load(std::istream& ifs);
  // convert binary to op data
//  void read(BlackT::TStream& ifs);
  // convert op data to binary
//  void write(BlackT::TStream& ofs);
};


}


#endif
