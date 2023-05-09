#include "dandy/DandyScriptReader.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>

using namespace BlackT;

namespace Psx {


//const static int scriptBufferCapacity = 0x10000;
const static int literalSize = 5;

DandyScriptReader::DandyScriptReader(
                  BlackT::TStream& src__,
                  FileToResultMap& dst__,
                  const BlackT::TThingyTable& thingy__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    lineNum(0),
    breakTriggered(false),
    fileNum(0),
    currentScriptWidth(0),
    currentScriptBuffer() {
  loadThingy(thingy__);
//  spaceOfs.open((outprefix + "msg_space.txt").c_str());
//  indexOfs.open((outprefix + "msg_index.txt").c_str());
  resetScriptBuffer();
}

bool DandyScriptReader::operator()() {
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
  
void DandyScriptReader::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}
  
void DandyScriptReader::outputNextSymbol(TStream& ifs) {
  // literal value
  if ((ifs.remaining() >= literalSize)
      && (ifs.peek() == '<')) {
    int pos = ifs.tell();
    
    ifs.get();
    if (ifs.peek() == '$') {
      ifs.get();
      std::string valuestr = "0x";
//      valuestr += ifs.get();
//      valuestr += ifs.get();
      valuestr += ifs.get();
      valuestr += ifs.get();
      
      if (ifs.peek() == '>') {
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
  
    int symbolSize;
    if (result.id <= 0xFF) symbolSize = 1;
    else if (result.id <= 0xFFFF) symbolSize = 2;
    else if (result.id <= 0xFFFFFF) symbolSize = 3;
    else symbolSize = 4;
//    int symbolSize = 2;
//    int symbolSize = result.size;
    
    // HACK: parametered ops
    if ((result.id == 0x5C40)    // \@
        || (result.id == 0x5C6D) // \m
        || (result.id == 0x5C70) // \p
        || (result.id == 0x5C73) // \s
        || (result.id == 0x5C76) // \v
        ) {
      currentScriptBuffer.writeInt(result.id, symbolSize,
        EndiannessTypes::big, SignednessTypes::nosign);
      
      // blindly copy param chars
      // FIXME: the \p op does not use a fixed 2-byte parameter size.
      // this should have copied characters until a non-digit or EOF was
      // found.
      // too bad i didn't figure this out until the patch was already
      // released!
      // as a result, the \p op is broken for values higher than 99.
      currentScriptBuffer.put(ifs.get());
      currentScriptBuffer.put(ifs.get());
    }
    // HACK: other backslashed stuff
    else if ((result.id & 0xFF00) == 0x5C00) {
      currentScriptBuffer.writeInt(result.id, symbolSize,
        EndiannessTypes::big, SignednessTypes::nosign);
    }
    else {
      // TEMP??
      // convert to 16-bit game-character encoding
//      currentScriptBuffer.writeu8(0x80);
//      currentScriptBuffer.writeInt(result.id + 1, symbolSize,
//        EndiannessTypes::big, SignednessTypes::nosign);
      currentScriptBuffer.writeInt(result.id, symbolSize,
        EndiannessTypes::big, SignednessTypes::nosign);
    }
    
    return;
  }
  
  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
//  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "DandyScriptReader::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'");
}
  
void DandyScriptReader::flushActiveScript() {
  // write terminator
//  currentScriptBuffer.put(0x00);

  int outputSize = currentScriptBuffer.size();
  
  ResultString result;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
  result.srcOffset = currentScriptSrcOffset;
  result.srcSize = currentScriptSrcSize;
  result.srcOpOffset = currentScriptSrcOpOffset;
  result.index = currentScriptIndex;
  result.name = currentScriptName;
  result.width = currentScriptWidth;
  
  dst[fileNum].push_back(result);
  
  // clear script buffer
  resetScriptBuffer();
}

void DandyScriptReader::resetScriptBuffer() {
  currentScriptBuffer = TBufStream();
  currentScriptSrcOffset = -1;
}
  
/*bool DandyScriptReader::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
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

void DandyScriptReader::processDirective(BlackT::TStream& ifs) {
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
    processStartMsg(ifs);
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
  else if (name.compare("STARTFILE") == 0) {
    processStartFile(ifs);
  }
  else if (name.compare("ENDFILE") == 0) {
    processEndFile(ifs);
  }
  else if (name.compare("SETLASTXPOS") == 0) {
    currentScriptWidth = TParse::matchInt(ifs);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "DandyScriptReader::processDirective()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Unknown directive: "
                              + name);
  }
  
  TParse::matchChar(ifs, ')');
}

void DandyScriptReader::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void DandyScriptReader::processStartMsg(BlackT::TStream& ifs) {
  if (TParse::checkInt(ifs)) {
    currentScriptSrcOffset = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcSize = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcOpOffset = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptIndex = TParse::matchInt(ifs);
  }
  else if (TParse::checkString(ifs)) {
    currentScriptName = TParse::matchString(ifs);
  }
}

void DandyScriptReader::processEndMsg(BlackT::TStream& ifs) {
  flushActiveScript();
}

void DandyScriptReader::processIncBin(BlackT::TStream& ifs) {
  std::string filename = TParse::matchString(ifs);
  TBufStream src(1);
  src.open(filename.c_str());
  currentScriptBuffer.writeFrom(src, src.size());
}

void DandyScriptReader::processBreak(BlackT::TStream& ifs) {
  breakTriggered = true;
}

void DandyScriptReader::processStartFile(BlackT::TStream& ifs) {
  flushActiveScript();
  fileNum = TParse::matchInt(ifs);
}

void DandyScriptReader::processEndFile(BlackT::TStream& ifs) {
  TParse::matchInt(ifs);
//  TParse::matchInt(ifs);
////  flushActiveScript();
//  breakTriggered = true;
}


}
