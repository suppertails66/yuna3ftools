#include "gr/GrPacCmp.h"
#include <iostream>

using namespace BlackT;

namespace Psx {


int GrPacCmp::decmpLz(BlackT::TStream& src, BlackT::TStream& dst,
              int remaining) {
  
  int command = 0;
  int counter = 0;
  loadCommandBits(src, command, counter, remaining);
  
  while (true) {
//    std::cerr << "pos: " << std::hex << src.tell() << " " << dst.tell() << std::endl;
    int bit = fetchBit(src, command, counter, remaining);
    
    // 1: literal
    if (bit != 0) {
//      std::cerr << "literal" << std::endl;
      dst.put(src.get());
      --remaining;
      continue;
    }
    
    bit = fetchBit(src, command, counter, remaining);
    
    // 00: short lookback
    if (bit == 0) {
//      std::cerr << "short lookback" << std::endl;
      int runlen = 0;
      runlen |= fetchBit(src, command, counter, remaining);
      runlen <<= 1;
      runlen |= fetchBit(src, command, counter, remaining);
      
      int distance = src.readu8();
      --remaining;
      
      lookbackCopy(dst, distance, runlen + 2);
      
      continue;
    }
    
    // 01: long lookback
    
    int low = src.readu8();
    int high = src.readu8();
    remaining -= 2;
    int runlen = (high & 0x07);
    int distance = ((high & 0xF8) << 5) + low;
    
    if (runlen != 0) {
//      std::cerr << "long lookback" << std::endl;
//      std::cerr << "  " << distance << " " << runlen + 2 << std::endl;
//      return remaining;
      lookbackCopy(dst, distance, runlen + 2);
    }
    else {
//      std::cerr << "extended long lookback" << std::endl;
      // if distance is zero, this is either an extended lookback or the
      // terminator
      
      int next = src.readu8();
      --remaining;
      // 00 = terminator
      if (next == 0x00) break;
      
      // 01 = command ignored, for some reason
      if (next == 0x01) continue;
      
      // otherwise, extended lookback
      lookbackCopy(dst, distance, next + 1);
    }
  }
  
  
  return remaining;
}

inline int GrPacCmp::fetchBit(
    BlackT::TStream& src, int& command, int& counter, int& remaining) {
  int bit = (command & 0x1);
  command >>= 1;
  --counter;
  if (counter == 0) {
    loadCommandBits(src, command, counter, remaining);
  }
  return bit;
}

inline void GrPacCmp::loadCommandBits(
    BlackT::TStream& src, int& command, int& counter, int& remaining) {
  command = src.readu16le();
  counter = 0x10;
  remaining -= 2;
}

inline void GrPacCmp::lookbackCopy(BlackT::TStream& dst,
                                int distance, int runlen) {
  // are you still making this dumb mistake?
//  char buffer[maxRunLength];
//  int dstpos = dst.tell();
//  dst.seekoff(-distance);
//  dst.read(buffer, runlen);
//  dst.seek(dstpos);
//  dst.write(buffer, runlen);
  
  int dstpos = dst.tell();
  for (int i = 0; i < runlen; i++) {
    dst.seekoff(-distance);
    char next = dst.get();
    dst.seek(dstpos);
    dst.put(next);
    ++dstpos;
  }
}


}
