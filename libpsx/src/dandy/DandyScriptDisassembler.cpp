#include "dandy/DandyScriptDisassembler.h"
#include "dandy/DandyScriptOp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Psx {


DandyScriptDisassembler::DandyScriptDisassembler(DandyScriptEnv& env__,
                          BlackT::TStream& ifs__,
                          std::ostream& ofs__)
  : env(env__),
    ifs(ifs__),
    ofs(ofs__) { };

void DandyScriptDisassembler::operator()() {
  std::vector<DandyScriptOp> ops;

  while (!ifs.eof()) {
    int pos = ifs.tell();
    
    try {
      DandyScriptOp op;
      op.read(env, ifs);
      
//      op.save(env, ofs);
      ops.push_back(op);
    }
    catch (TGenericException& e) {
      std::cerr << "Error disassembling op at offset 0x" << std::hex << pos
        << ":" << std::endl;
      std::cerr << e.problem() << std::endl;
//      throw e;
      break;
    }
    catch (TException& e) {
      std::cerr << "Error disassembling op at offset 0x" << std::hex << pos
        << ":" << std::endl;
      std::cerr << e.what() << std::endl;
//      throw e;
      break;
    }
  }
  
  for (std::vector<DandyScriptOp>::iterator it = ops.begin();
       it != ops.end();
       ++it) {
    DandyScriptOp& op = *it;
    
    // check environment for any needed labels and print them
    for (DandyScriptEnv::LabelCollection::iterator it = env.labels.begin();
         it != env.labels.end();
         ++it) {
      DandyScriptLabel& label = *it;
      if (label.value == op.getSrcPos()) {
        ofs << std::endl;
        ofs << ":" << label.name << std::endl;
      }
    }
    
    ofs << "  ";
    op.save(env, ofs);
  }
}


}
