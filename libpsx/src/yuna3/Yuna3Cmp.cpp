#include "yuna3/Yuna3Cmp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {

void Yuna3Cmp::decmpSs1(BlackT::TStream& ifs, BlackT::TStream& ofs,
                        int outputSize) {
  //; halfword-based rle:
  //; - first byte nonzero, top bit unset = write X occurrences of the following halfword
  //; - top bit set or byte is zero = copy one literal halfword??
  //; man these guys use the wackiest graphics compressions
  
  int remaining = outputSize;
  bool done = false;
  while (!done && !ifs.eof()) {
    int next = (unsigned char)ifs.peek();
    
    int count;
    int value;
    if ((next != 0) && ((next & 0x80) == 0)) {
      ifs.get();
      count = next;
    }
    else {
      count = 1;
    }
    
    value = ifs.readu16be();
    for (int i = 0; i < count; i++) {
      // reverse endianness
      ofs.writeu16le(value);
      
      if (outputSize != -1) {
        remaining -= 2;
        if (remaining <= 0) {
          done = false;
          break;
        }
      }
    }
  }
  
//  std::cerr << std::hex << remaining << std::endl;
}

void Yuna3Cmp::cmpSs1(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  while (!ifs.eof()) {
    int basePos = ifs.tell();
    // NOTE: assumes input is little-endian (i.e. playstation format)
    int nextWord = ifs.readu16le();
    
    bool forceLiteral = false;
//    if ((nextWord & 0x8000) != 0)
//      forceLiteral = true;
    
    int numRepeats = 0;
//    if (!forceLiteral) {
      numRepeats = countWordRepeats(ifs, nextWord);
//    }
    
//    std::cerr << std::hex << nextWord << ": " << numRepeats << std::endl;
//    char c;
//    std::cin >> c;
    
    if (numRepeats > 0x7E) numRepeats = 0x7E;
    
    if (numRepeats < 1) {
      forceLiteral = true;
    }
    
    if (forceLiteral) {
      nextWord |= 0x8000;
      ofs.writeu16be(nextWord);
    }
    else {
      nextWord |= 0x8000;
      ofs.writeu8(numRepeats + 1);
      ofs.writeu16be(nextWord);
      // skip repeats in input
      ifs.seekoff(numRepeats * 2);
    }
  }
}

int Yuna3Cmp::countNextWordRepeats(BlackT::TStream& ifs) {
  if (ifs.eof()) return -1;
  
  int basePos = ifs.tell();
  int nextWord = ifs.readu16le();
  int result = countWordRepeats(ifs, nextWord);
  ifs.seek(basePos);
  
  return result;
}

int Yuna3Cmp::countWordRepeats(BlackT::TStream& ifs, int value) {
  int basePos = ifs.tell();
  int nextWord = value;
  int count = 0;
  while (!ifs.eof()) {
    int next = ifs.readu16le();
    if (next != nextWord) break;
    ++count;
  }
  
  ifs.seek(basePos);
  return count;
}


}
