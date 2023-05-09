#include "dandy/DandyScriptAssembler.h"
#include "dandy/DandyScriptOpStereotypes.h"
#include "dandy/DandyScriptOp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include "util/TFileManip.h"
#include "exception/TGenericException.h"
#include <iostream>
#include <exception>

using namespace BlackT;

namespace Psx {


DandyScriptAssembler::DandyScriptAssembler(DandyScriptAsmEnv& env__,
//                       std::istream& src__,
                       BlackT::TStream& src__,
                       BlackT::TStream& ofs__)
  : env(env__),
    src(src__),
    ofs(ofs__),
    isInclude(false) { };

void DandyScriptAssembler::operator()() {
/*  try {
    while (!src.eof()) {
      std::string line;
      src.getLine(line);
      ++lineNum;
      
  //    std::cerr << lineNum << std::endl;
      if (line.size() <= 0) continue;
      
      // discard lines containing only ASCII spaces and tabs
  //    bool onlySpace = true;
  //    for (int i = 0; i < line.size(); i++) {
  //      if ((line[i] != ' ')
  //          && (line[i] != '\t')) {
  //        onlySpace = false;
  //        break;
  //      }
  //    }
  //    if (onlySpace) continue;
      
      TBufStream ifs(line.size());
      ifs.write(line.c_str(), line.size());
      ifs.seek(0);
      
      // check for special stuff
      if (ifs.peek() == '#') {
        // directives
        ifs.get();
        processDirective(ifs);
        
        if (breakTriggered) {
          breakTriggered = false;
          return false;
        }
        
        continue;
      }
      
      while (!ifs.eof()) {
        // check for comments
        if ((ifs.remaining() >= 2)
            && (ifs.peek() == '/')) {
          ifs.get();
          if (ifs.peek() == '/') break;
          else ifs.unget();
        }
        
        outputNextSymbol(ifs);
      }
    }
    
    if (currentScriptBuffer.size() > 0) {
      flushActiveScript();
    }
    
    return true;
  }
  catch (std::exception& e) {
    std::cerr << "Exception on script line " << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  } */
  
  int lineNum = 0;
  try {
    while (!src.eof()) {
      std::string line;
      src.getLine(line);
      ++lineNum;
      
  //    std::cerr << lineNum << std::endl;
      if (line.size() <= 0) continue;
      
      // discard lines containing only ASCII spaces and tabs
  //    bool onlySpace = true;
  //    for (int i = 0; i < line.size(); i++) {
  //      if ((line[i] != ' ')
  //          && (line[i] != '\t')) {
  //        onlySpace = false;
  //        break;
  //      }
  //    }
  //    if (onlySpace) continue;
      
      TBufStream ifs(line.size());
      ifs.write(line.c_str(), line.size());
      ifs.seek(0);
      
      TParse::skipSpace(ifs);
      // skip empty lines
      if (ifs.eof()) continue;
      
      // comment line?
      if (TParse::checkChar(ifs, ';')) {
        
      }
      // label definition?
      else if (TParse::checkChar(ifs, ':')) {
        TParse::matchChar(ifs, ':');
        
        DandyScriptLabel label;
        label.name = TParse::getSpacedSymbol(ifs);
        label.value = ofs.tell();
        env.addLabel(label);
        
//        std::cerr << ":" << name << std::endl;
      }
      // directive?
      else if (TParse::checkChar(ifs, '#')) {
        TParse::matchChar(ifs, '#');
        
        TParse::skipSpace(ifs);
        std::string name = TParse::matchName(ifs);
        TParse::matchChar(ifs, '(');
  
        for (int i = 0; i < name.size(); i++) {
          name[i] = toupper(name[i]);
        }
        
        if (name.compare("INCLUDE") == 0) {
          std::string fileName = TParse::matchString(ifs);
          if (!TFileManip::fileExists(fileName)) {
            throw TGenericException(T_SRCANDLINE,
                                    "DandyScriptAssembler::operator()",
                                    std::string("Included nonexistent file: ")
                                      + fileName);
          }
          
          // scoping!!
          // ...ah fuck it
//          DandyScriptAsmEnv extEnv = env;
          DandyScriptAsmEnv& extEnv = env;
          TBufStream extIfs;
          extIfs.open(fileName.c_str());
          
          DandyScriptAssembler assembler(extEnv, extIfs, ofs);
          assembler.isInclude = true;
          assembler.includedFileName = fileName;
  
          assembler();
        }
        else {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptAssembler::operator()",
                                  std::string("Unknown directive: ")
                                    + name);
        }
        
        TParse::matchChar(ifs, ')');
      }
      // otherwise, an opcode
      else {
        DandyScriptOp op;
        op.load(env, ifs);
        op.write(env, ofs);
      }
    }
    
    // resolve references
    if (!isInclude) {
      for (DandyScriptAsmEnv::ReferenceCollection::iterator it
             = env.references.begin();
           it != env.references.end();
           ++it) {
        DandyScriptReference reference = *it;
        
        // find matching label
        DandyScriptLabel* label = NULL;
        int num = 0;
        for (DandyScriptAsmEnv::LabelCollection::iterator it
               = env.labels.begin();
             it != env.labels.end();
             ++it) {
          if (it->name.compare(reference.name) == 0) {
            label = &(*it);
            break;
          }
          
          ++num;
        }
        
        if (label == NULL) {
          throw TGenericException(T_SRCANDLINE,
                                  "DandyScriptAssembler::()",
                                  std::string("Reference to unknown label: ")
                                    + reference.name);
        }
        
  //      std::cerr << reference.name << ": " << num << std::endl;
        ofs.seek(reference.offset);
        ofs.writeu8(num);
      }
    }
  }
  catch (TGenericException& e) {
    if (isInclude) {
      std::cerr << "In included file \""
        << includedFileName << "\": " << std::endl;
    }
    std::cerr << "Exception on script line "
      << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.problem() << std::endl;
    throw e;
  }
  catch (std::exception& e) {
    if (isInclude) {
      std::cerr << "In included file \""
        << includedFileName << "\": " << std::endl;
    }
    std::cerr << "Exception on script line "
      << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
  
}


}
