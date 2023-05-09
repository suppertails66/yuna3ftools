#include "pom/PomScriptReader.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>


// for testing: if set, unreconized symbols will be ignored
#define POMREADER_IGNORE_BAD_INPUT 1


using namespace BlackT;

namespace Psx {


//const static int scriptBufferCapacity = 0x10000;
const static int literalSize = 5;

const static int code_end     = 0x00;

PomScriptReader::ResultString::ResultString()
  : srcOffset(-1),
    srcPointer(-1),
    srcSize(-1),
    width(-1),
    isLiteral(false),
    isNotCompressible(false),
    scriptRefStart(-1),
    scriptRefEnd(-1),
    scriptRefCode(-1),
    mapMainNum(-1),
    mapSubNum(-1) { }

PomScriptReader::PomScriptReader(
                  BlackT::TStream& src__,
                  RegionToResultMap& dst__,
                  const BlackT::TThingyTable& thingy__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    lineNum(0),
    breakTriggered(false),
    regionNum(0),
    currentScriptSrcOffset(-1),
    currentScriptSrcSize(-1),
    currentScriptWidth(0),
    currentScriptIsLiteral(false),
    currentScriptIsNotCompressible(false),
    currentMapMainNum(-1),
    currentMapSubNum(-1),
    currentScriptBuffer() {
  loadThingy(thingy__);
//  spaceOfs.open((outprefix + "msg_space.txt").c_str());
//  indexOfs.open((outprefix + "msg_index.txt").c_str());
  resetScriptBuffer();
}

bool PomScriptReader::operator()() {
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
  catch (TGenericException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.problem() << std::endl;
    throw e;
  }
  catch (TException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
  catch (std::exception& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
}
  
void PomScriptReader::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}
  
void PomScriptReader::outputNextSymbol(TStream& ifs) {
  if (currentScriptIsLiteral) {
    currentScriptBuffer.put(ifs.get());
    return;
  }

  // literal value
  if ((ifs.remaining() >= literalSize)
      && (ifs.peek() == literalOpenSymbol)) {
    int pos = ifs.tell();
    
    ifs.get();
    if (ifs.peek() == '$') {
      ifs.get();
      std::string valuestr = "0x";
//      valuestr += ifs.get();
//      valuestr += ifs.get();
      valuestr += ifs.get();
      valuestr += ifs.get();
      
      if (ifs.peek() == literalCloseSymbol) {
        ifs.get();
        int value = TStringConversion::stringToInt(valuestr);
        
//        currentScriptBuffer.writeu16be(value);
        currentScriptBuffer.writeu8(value);

        return;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  TThingyTable::MatchResult result;
  result = thingy.matchTableEntry(ifs);
  
  if (result.id != -1) {
//    std::cerr << std::dec << lineNum << " " << std::hex << result.id << " " << result.size << std::endl;

    // HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
    // TThingyTable does not support mapping multiple characters to the
    // same ID, so i've done a quick workaround
    if ((result.id >= 0x100)) {
      result.id >>= 8;
    }
  
    int symbolSize;
    if (result.id <= 0xFF) symbolSize = 1;
    else if (result.id <= 0xFFFF) symbolSize = 2;
    else if (result.id <= 0xFFFFFF) symbolSize = 3;
    else symbolSize = 4;
    
    currentScriptBuffer.writeInt(result.id, symbolSize,
      EndiannessTypes::big, SignednessTypes::nosign);
    
    return;
  }

  #if POMREADER_IGNORE_BAD_INPUT
    // ignore unrecognized characters
    // (and handle sjis sequences)
    if (((ifs.readu8() & 0x80) != 0) && !ifs.eof()) ifs.get();
    return;
  #endif
  
  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
//  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "PomScriptReader::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'");
}
  
void PomScriptReader::flushActiveScript() {
//  if (currentScriptBuffer.size() <= 0) return;
  // empty strings are valid, so check for valid ID instead
  if (currentScriptSrcOffset < 0) return;
  
  // append terminator
  currentScriptBuffer.put(code_end);

  int outputSize = currentScriptBuffer.size();
  
  ResultString result = currentResult;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
  result.srcOffset = currentScriptSrcOffset;
  result.srcSize = currentScriptSrcSize;
//  result.srcOpOffset = currentScriptSrcOpOffset;
//  result.index = currentScriptIndex;
//  result.name = currentScriptName;
  result.width = currentScriptWidth;
//  result.indices = currentScriptIndices;
  result.isLiteral = currentScriptIsLiteral;
  result.isNotCompressible = currentScriptIsNotCompressible;
  result.mapMainNum = currentMapMainNum;
  result.mapSubNum = currentMapSubNum;
  
  dst[regionNum].push_back(result);
  
  // clear script buffer
  resetScriptBuffer();
}

void PomScriptReader::resetScriptBuffer() {
  currentScriptBuffer = TBufStream();
  currentScriptSrcOffset = -1;
  currentScriptSrcSize = -1;
  currentScriptIsLiteral = false;
//  currentScriptIndices.clear();
  currentResult = ResultString();
}
  
/*bool PomScriptReader::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
  if (symbol.size() > ifs.remaining()) return false;
  
  int startpos = ifs.tell();
  for (int i = 0; i < symbol.size(); i++) {
    if (symbol[i] != ifs.get()) {
      ifs.seek(startpos);
      return false;
    }
  }
  
  return true;
} */

void PomScriptReader::processDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("LOADTABLE") == 0) {
    processLoadTable(ifs);
  }
  else if (name.compare("STARTSTRING") == 0) {
    currentResult.id = TParse::matchString(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcOffset = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcSize = TParse::matchInt(ifs);
//    processStartMsg(ifs);
  }
  else if (name.compare("ENDSTRING") == 0) {
    processEndMsg(ifs);
  }
  else if (name.compare("INCBIN") == 0) {
    processIncBin(ifs);
  }
  else if (name.compare("BREAK") == 0) {
    processBreak(ifs);
  }
  else if (name.compare("STARTREGION") == 0) {
    processStartRegion(ifs);
  }
  else if (name.compare("ENDREGION") == 0) {
    processEndRegion(ifs);
  }
  else if (name.compare("SETLASTXPOS") == 0) {
    currentScriptWidth = TParse::matchInt(ifs);
  }
  else if (name.compare("SETMAP") == 0) {
    currentMapMainNum = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentMapSubNum = TParse::matchInt(ifs);
  }
  else if (name.compare("SETSCRIPTREF") == 0) {
    currentResult.scriptRefStart = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentResult.scriptRefEnd = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentResult.scriptRefCode = TParse::matchInt(ifs);
  }
  else if (name.compare("ADDPOINTERREF") == 0) {
    currentResult.pointerRefs.push_back(TParse::matchInt(ifs));
  }
  else if (name.compare("ADDOVERWRITE") == 0) {
    currentResult.overwriteAddresses.push_back(TParse::matchInt(ifs));
  }
  else if (name.compare("SETNOTCOMPRESSIBLE") == 0) {
    currentScriptIsNotCompressible = (TParse::matchInt(ifs) != 0);
  }
//  else if (name.compare("SETINDEXLIST") == 0) {
//    processSetIndexList(ifs);
//  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "PomScriptReader::processDirective()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Unknown directive: "
                              + name);
  }
  
  TParse::matchChar(ifs, ')');
}

void PomScriptReader::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void PomScriptReader::processStartMsg(BlackT::TStream& ifs) {
/*  currentScriptSrcOffset = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptSrcPointer = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptSrcSize = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptIsLiteral = (TParse::matchInt(ifs) != 0); */
}

void PomScriptReader::processEndMsg(BlackT::TStream& ifs) {
  flushActiveScript();
}

void PomScriptReader::processIncBin(BlackT::TStream& ifs) {
  std::string filename = TParse::matchString(ifs);
  TBufStream src(1);
  src.open(filename.c_str());
  currentScriptBuffer.writeFrom(src, src.size());
}

void PomScriptReader::processBreak(BlackT::TStream& ifs) {
  breakTriggered = true;
}

void PomScriptReader::processStartRegion(BlackT::TStream& ifs) {
  flushActiveScript();
  regionNum = TParse::matchInt(ifs);
//  TParse::matchChar(ifs, ',');
//  int pointerTableBase = TParse::matchInt(ifs);
}

void PomScriptReader::processEndRegion(BlackT::TStream& ifs) {
  TParse::matchInt(ifs);
//  TParse::matchInt(ifs);
////  flushActiveScript();
//  breakTriggered = true;
}

/*void PomScriptReader::processSetIndexList(BlackT::TStream& ifs) {
  currentScriptIndices.clear();
  while (TParse::checkInt(ifs)) {
    int next = TParse::matchInt(ifs);
    currentScriptIndices.push_back(next);
    if (!TParse::checkChar(ifs, ',')) break;
    TParse::matchChar(ifs, ',');
  }
} */


}
