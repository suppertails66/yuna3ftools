#include "dandy/DandyCmp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {

/* start:
    // for the first command ONLY:
    // * cmd >= 0x15 is a pseudo-extended absolute run:
    //   copy the first (cmd - 0x11) bytes
    // * cmd 0x12, 0x13, 0x14 are a short copy of 1-3 bytes
    //   followed by a mode2 lookback
    // * cmd < 0x12 is a standard command
    
    - fetch byte = cmd
    - if cmd < 0x12: goto mode1
    - if (0x12 <= cmd < 0x15):
      - length = (cmd - 0x11)
      - goto shortCopySub
    - if (cmd >= 0x15):
      - length = (cmd - 0x11)
      - copy length bytes from src to dst
      - goto mode1Lookback
  
  mode1:
    - fetch byte = cmd
    - if cmd >= 0x10: goto mode2
    // 0x00-0x0F
    - derive size:
      - if 0, this is an extended size:
          - advance src until a nonzero byte is found.
            for each zero byte, add 0xFF to a counter.
          - fetch terminating nonzero byte = sizelo
          - add 0xF to counter, then add sizelo to get total size
      - else size = cmd
    - copy raw content:
      - copy (size + 3) bytes from input to output
    - fetch byte = cmd2
  mode1Lookback:
    - if cmd2 >= 0x10: goto mode2
    - do lookback:
      - fetch byte from src = offset
      - copysrc = (dst - 0x801) - (cmd2 >> 2) - (offset << 2)
      - copy 3 bytes from copysrc to dst
    - goto shortCopy (with src-2 = cmd2)
  
  mode2:
    // quick lookback (0x1-0x800 bytes back, length 3-8 bytes?)
    // in this case, the low two bits of the command byte form the
    // "short copy" length (rather than having it in a separate length byte)
    - if (cmd >= 0x40): // 0x40-0xFF
      - fetch byte = subcmd
      - copysrc = (dst - 1) - ((cmd >> 2) & 0x7) - (subcmd << 3)
      - length = (cmd >> 5) - 1
      - goto copyLookback
    // medium lookback (0x1-0x4000 bytes back)
    - if (cmd >= 0x20): // 0x20-0x3F
      - length = (cmd & 0x1F)
      - if length == 0, this is an extended length:
        - advance src until a nonzero byte is found.
          for each zero byte, add 0xFF to a counter.
        - fetch terminating nonzero byte = sizelo
        - add 0x1F to counter, then add sizelo to get total size
      - byte0 = fetch byte
      - byte1 = fetch byte
      - copysrc = (dst - 1) - ((byte0 >> 2) + (byte1 << 6))
      - goto copyLookback
    // far lookback (0x4001-0xBFFF bytes backward).
    // also used as data terminator (0x17 0x00 0x00 or any other command
    // that would nominally copy data from exactly 0x4000 bytes backward)
    - if (cmd >= 0x10): // 0x10-0x1F
      - length = (cmd & 0x7)
      - if length == 0, this is an extended length:
        - advance src until a nonzero byte is found.
          for each zero byte, and for the terminating nonzero byte,
          add 0xFF to a counter.
        - fetch terminating nonzero byte = sizelo
        - add 0x7 to counter, then add sizelo to get total size
      - byte0 = fetch byte
      - byte1 = fetch byte
      // the two fetched bytes are combined with the command byte to form
      // a 15-bit offset (up to 0x3FFF)
      - copysrc = dst - ((cmd & 0x8) << 11) - ((byte0 >> 2) + (byte1 << 6))
      - if copysrc == dst: goto done
      - else:
        - copysrc -= 0x4000
        - goto copyLookback
    // this can happen *only* after the very first command,
    // if it is a special 1 to 3 byte copy
    - if (cmd < 0x10): // 0x00-0x0F
      - fetch byte
      - copysrc = (dst - 1) - (cmd >> 2) - (byte << 2)
      - copy 2 bytes from copysrc to dst
      - goto shortCopy
  
  copyLookback:
    // NOTE: there is an optimization here where the game will copy
    // by 4-byte words if the src/dst addresses are both divisible by 4,
    // and the lookback length is greater than 6(?).
    // however, mathematically, this does not interfere with the functionality
    // of loop-copying across the initial dst pointer... i hope
    - copy (length + 2) bytes from copysrc to dst
    - goto shortCopy

  // if the two low bits of the first length byte are zero,
  // read another full command.
  // otherwise, copy that many absolute bytes from src,
  // then do another lookback.
  shortCopy:
    - if (src-2 & 0x03) == 0: goto mode1
    - if (src-2 & 0x03) != 0:
  shortCopySub:
      - copy (cmd2 & 0x03) bytes from src to dst
      - goto mode2 (next byte = cmd)
  
  done: */


void DandyCmp::cmp(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  int ofsBase = ofs.tell();
  
  int uncompressedSize = ifs.remaining();
  
  // reserve space for header
  ofs.seekoff(0x10);
  
  int dataStart = ofs.tell();
  
    // "compress" data as one long absolute command
/*    ofs.put(0);
    int reportedLength = uncompressedSize - 0xF - 3;
    while (reportedLength >= (0xFF + 1)) {
      ofs.put(0);
      reportedLength -= 0xFF;
    }
    ofs.put(reportedLength);
    
    // output entire data uncompressed
    while (!ifs.eof()) ofs.put(ifs.get()); */
    
    // the way this compression works is that every absolute command
    // *must* be followed by a lookback.
    // both absolute and lookbacks can be of unlimited length, but you
    // must know where your next lookback goes ahead of time.
    
    bool onFirstCmd = true;
    int curAbsoluteStartPos = 0;
    StringCacheTable cache;
    int lastCacheClean = 0;
    
    while (!ifs.eof()) {
//      std::cerr << std::hex << ifs.tell() << " / " << ifs.size() << " : " << ifs.remaining() << std::endl;
    
      //=====================
      // special case:
      // check for repeated
      // sequences at current
      // position
      //=====================
      
      // NOTE: this achieves decent speedup but reduces efficiency
      // (possibly more efficient lookbacks are ignored in favor of
      // the immediately recognized repeat)
      
      if (!onFirstCmd && (minRepeatEfficiency > 0)) {
        RepSearchResult bestRepeat = findBestRepeat(ifs);
        int repeatEfficiency = bestRepeat.seqLen * bestRepeat.numRepeats;
        if (repeatEfficiency >= minRepeatEfficiency) {
  //        std::cerr << std::hex << ifs.tell() << " " << ofs.tell() << std::endl;
  //        char c;
  //        std::cin >> c;
          
          // create absolute command with the target characters
          writeAbsoluteCommand(ifs, ofs, bestRepeat.seqLen);
          // create lookback covering those characters as many times as needed
          writeLookbackCommand(ifs, ofs, bestRepeat.seqLen, repeatEfficiency);
          
          // skip content covered by the lookback
          //ifs.seekoff(lookbackSize);
          for (int i = 0; i < repeatEfficiency; i++) {
            addCacheEntry(ifs, cache);
            ifs.seekoff(1);
          }
          
          continue;
        }
      }
    
      //=====================
      // absolute
      //=====================
      
      curAbsoluteStartPos = ifs.tell();
      
/*      if (curAbsoluteStartPos >= stringTableCacheEntrySize) {
        int newEntryPos = curAbsoluteStartPos - stringTableCacheEntrySize;
        ifs.seek(newEntryPos);
        std::string newEntryString;
        for (int i = 0; i < stringTableCacheEntrySize; i++)
          newEntryString += ifs.get();
        
        cache[newEntryString][newEntryPos] = 0;
      }
      
      ifs.seek(curAbsoluteStartPos); */
      
      if (autoCacheEmptyThreshold != 0) {
        if (cache.size() >= autoCacheEmptyThreshold) cache.clear();
      }
      
      if (autoCacheCleanThreshold != 0) {
//        if (cache.size() >= autoCacheCleanThreshold) {
//          cleanCache(cache, ifs.tell());
//        }
        if ((ifs.tell() - lastCacheClean) >= autoCacheCleanThreshold) {
          cleanCache(cache, ifs.tell());
          lastCacheClean = ifs.tell();
        }
      }
      
      // evaluate the best lookbacks at successive positions until we
      // discover one that is more efficient than adding to the
      // absolute run
      
      CmpSearchResult bestLookback;
      bestLookback.address = 0;
      bestLookback.length = 0;
      
//      std::cerr << "here1" << std::endl;
//      std::cerr << "here2" << std::endl;
      while (!ifs.eof()) {
        addCacheEntry(ifs, cache);
        
//        CmpSearchResult newLookback = findBestLookback(ifs);
//        if (newLookback.length >= bestLookback.length) {
//          bestLookback.length = newLookback.length;
//        }
        bestLookback = findBestLookback(ifs, cache);
        
        // if we found an efficient lookback, we're done
        if (bestLookback.length >= minEfficientLookbackSize) break;
        
        ifs.seekoff(1);
      }
      
      // if we hit EOF and have no lookback, we're done
//      if (ifs.eof() && (bestLookback.length < minEfficientLookbackSize
      
      int absoluteRunLen = ifs.tell() - curAbsoluteStartPos;
      ifs.seek(curAbsoluteStartPos);

      // encode absolute sequence
      if (onFirstCmd) {
        // absoluteRunLen should never be zero on the first run
        
        writeAbsoluteCommandFirstRun(ifs, ofs, absoluteRunLen);
        onFirstCmd = false;
      }
      else {
//        std::cerr << std::hex << ifs.tell() << " " << ofs.tell() << " " << absoluteRunLen << std::endl;
//        char c;
//        std::cin >> c;
        
        writeAbsoluteCommand(ifs, ofs, absoluteRunLen);
      }
      
      // if we've hit EOF, we're done
      if (ifs.eof()) break;
      
      //=====================
      // lookback
      //=====================
      
      if (bestLookback.length < minEfficientLookbackSize) {
        throw TGenericException(T_SRCANDLINE,
                                "DandyCmp::cmp()",
                                "Computed too-short lookback length: "
                                + TStringConversion::intToString(
                                    bestLookback.length));
      }
      
      int absoluteLookbackDistance = ifs.tell() - bestLookback.address;
      
/*      if (absoluteLookbackDistance <= 0x4000) {
        writeFullShortLookbackCmd(ifs, ofs, absoluteLookbackDistance,
                                  bestLookback.length);
      }
      else if (absoluteLookbackDistance <= 0xBFFF) {
        writeLongLookbackCmd(ifs, ofs, absoluteLookbackDistance,
                                  bestLookback.length);
      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "DandyCmp::cmp()",
                                "Computed too-long lookback length: "
                                + TStringConversion::intToString(
                                    absoluteLookbackDistance));
      }
      
      // skip content covered by the lookback
      ifs.seekoff(bestLookback.length); */
      
      writeLookbackCommand(ifs, ofs, absoluteLookbackDistance,
                           bestLookback.length);
  
      // skip content covered by the lookback
      //ifs.seekoff(lookbackSize);
      for (int i = 0; i < bestLookback.length; i++) {
        addCacheEntry(ifs, cache);
        ifs.seekoff(1);
      }
    }
    
  // terminator
  ofs.put(0x17);
  ofs.put(0x00);
  ofs.put(0x00);
  
  int dataEnd = ofs.tell();
  
  // DEBUG
//  ifs.seek(0x8800);
//  CmpSearchResult test = findBestLookback(ifs);
//  std::cerr << std::hex << test.address << " " << test.length << std::endl;
  
  // header
  ofs.seek(ofsBase);
  ofs.put('B');
  ofs.put('a');
  ofs.put('B');
  ofs.put(0x00);
//  ofs.seekoff(4);
  ofs.writeu32le(0);
//  ofs.writeu32le(dataEnd - dataStart);
  ofs.writeu32le(uncompressedSize);
  ofs.writeu32le(0);
  ofs.seek(dataEnd);
}

void DandyCmp::cleanCache(StringCacheTable& cache, int currentPos) {
  int searchStartPos = currentPos - maxLookbackRange;
  if (searchStartPos < 0) searchStartPos = 0;  
  
  StringCacheTable::iterator cacheIt = cache.begin();
  while (cacheIt != cache.end()) {
    StringCacheSubTable& subTable = cacheIt->second;
    
    // delete any out-of-range table entries
//    bool hasValidEntry = false;
    if (subTable.size() > 0) {
      // search in reverse order so we can stop when out of range
      StringCacheSubTable::iterator searchIt = subTable.end();
      --searchIt;
      
      while (true) {
        // stop when we find an entry beyond the start of the search window
//        int checkAddr = searchIt->first;
        int checkAddr = *searchIt;
        if (checkAddr < searchStartPos) {
          // do nothing: entire table will be deleted
//          if (!hasValidEntry) break;
          
          // delete everything to the "left" of this position
          while (true) {
            bool last = false;
            if (searchIt == subTable.begin()) last = true;
            
            StringCacheSubTable::iterator deleteIt = searchIt;
            if (!last) --searchIt;
            subTable.erase(deleteIt);
            if (last) break;
          }
          break;
        }
        else {
//          hasValidEntry = true;
        }
        
        if (searchIt == subTable.begin()) break;
        --searchIt;
      }
    }
    
    // delete entire table if empty
    if ((subTable.size() == 0)
//        || !hasValidEntry
        ) {
      StringCacheTable::iterator next = cacheIt;
      ++next;
      cache.erase(cacheIt);
      cacheIt = next;
    }
    else {
      ++cacheIt;
    }
  }
}

void DandyCmp::addCacheEntry(BlackT::TStream& ifs,
                          StringCacheTable& cache) {
  int curAbsoluteStartPos = ifs.tell();
  
  if (curAbsoluteStartPos >= stringTableCacheEntrySize) {
    int newEntryPos = curAbsoluteStartPos - stringTableCacheEntrySize;
    ifs.seek(newEntryPos);
    std::string newEntryString;
    for (int i = 0; i < stringTableCacheEntrySize; i++)
      newEntryString += ifs.get();
    
//    cache[newEntryString][newEntryPos] = 0;
    cache[newEntryString].push_back(newEntryPos);
  }
  
  ifs.seek(curAbsoluteStartPos);
}


void DandyCmp::writeLookbackCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize) {
  if ((absoluteLookbackDistance <= 0x800)
      && (lookbackSize >= 3) && (lookbackSize <= 8)) {
    writeMiniShortLookbackCmd(ifs, ofs, absoluteLookbackDistance,
                              lookbackSize);
  }
  else if (absoluteLookbackDistance <= 0x4000) {
    writeFullShortLookbackCmd(ifs, ofs, absoluteLookbackDistance,
                              lookbackSize);
  }
  else if (absoluteLookbackDistance <= 0xBFFF) {
    writeLongLookbackCmd(ifs, ofs, absoluteLookbackDistance,
                              lookbackSize);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::cmp()",
                            "Computed too-long lookback length: "
                            + TStringConversion::intToString(
                                absoluteLookbackDistance));
  }
  // skip content covered by the lookback
  //ifs.seekoff(lookbackSize);
}

void DandyCmp::writeLongLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize) {
  // note that commands encoding a distance of 0x4000 are technically valid,
  // but actually act as the stream terminator.
  // this is treated as an error for our purposes.
  if ((absoluteLookbackDistance < 0x4001)
      || (absoluteLookbackDistance > 0xBFFF)) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::writeLongLookbackCmd()",
                            "Out-of-range target for long lookback: "
                            + TStringConversion::intToString(
                                absoluteLookbackDistance));
  }
  
  if ((lookbackSize < 3)) {
    throw TGenericException(T_SRCANDLINE,
                    "DandyCmp::writeLongLookbackCmd()",
                    "Tried to encode impossibly short long lookback run");
  }
  
  TByte cmd = 0x00;
  int cmdPos = ofs.tell();
  // write placeholder for command byte
  ofs.put(0x00);
  
  int reportedLen = lookbackSize - 2;
  if (reportedLen > 0x7) {
    reportedLen -= 0x7;
    // add one null byte per 0xFF bytes of input
    while (reportedLen > 0xFF) {
      ofs.put(0x00);
      reportedLen -= 0xFF;
    }
    
    // write final amount
    ofs.writeu8(reportedLen);
    
    reportedLen = 0;
  }
  
//  int offsetDataPos = ofs.tell();
  cmd = 0x10 + reportedLen;
  
  // write offset data
  int targetOffset = absoluteLookbackDistance - 0x4000;
  TByte offsetByte0 = 0x00;
  TByte offsetByte1 = 0x00;
  // highest bit of 15-bit offset amount goes in cmd
  cmd |= ((targetOffset >> 11) & 0x08);
  // remaining bits are shuffled into the offset bytes.
  // the low 2 bits of the first byte must remain clear --
  // they are used to signal a 1-3 byte absolute run (and
  // will be modified in the output stream if we subsequently
  // determine this is needed)
  offsetByte0 |= ((targetOffset & 0x003F) << 2);
  offsetByte1 |= ((targetOffset & 0x3FC0) >> 6);
  ofs.writeu8(offsetByte0);
  ofs.writeu8(offsetByte1);
  
  // write finalized command byte
  int finalPos = ofs.tell();
  ofs.seek(cmdPos);
  ofs.writeu8(cmd);
  ofs.seek(finalPos);
}

void DandyCmp::writeMiniShortLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize) {
  if ((absoluteLookbackDistance < 0x1)
      || (absoluteLookbackDistance > 0x800)) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::writeMiniShortLookbackCmd()",
                            "Out-of-range target for mini short lookback: "
                            + TStringConversion::intToString(
                                absoluteLookbackDistance));
  }
  
  if ((lookbackSize < 3)
      || (lookbackSize > 8)) {
    throw TGenericException(T_SRCANDLINE,
                    "DandyCmp::writeMiniShortLookbackCmd()",
                    "Tried to encode impossibly short mini lookback run");
  }
  
  TByte cmd = 0x00;
  TByte offsetByte0 = 0x00;
  
  int targetOffset = absoluteLookbackDistance - 1;
  offsetByte0 |= ((targetOffset & 0x07F8) >> 3);
  cmd |= ((targetOffset & 0x07)) << 2;
  
  int reportedLen = lookbackSize - 2 + 1;
  cmd |= (reportedLen << 5);
  
  ofs.writeu8(cmd);
  ofs.writeu8(offsetByte0);
}

void DandyCmp::writeFullShortLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize) {
  if ((absoluteLookbackDistance < 0x1)
      || (absoluteLookbackDistance > 0x4000)) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::writeFullShortLookbackCmd()",
                            "Out-of-range target for full short lookback: "
                            + TStringConversion::intToString(
                                absoluteLookbackDistance));
  }
  
  if ((lookbackSize < 3)) {
    throw TGenericException(T_SRCANDLINE,
                    "DandyCmp::writeFullShortLookbackCmd()",
                    "Tried to encode impossibly short short lookback run");
  }
  
  TByte cmd = 0x00;
  int cmdPos = ofs.tell();
  // write placeholder for command byte
  ofs.put(0x00);
  
  int reportedLen = lookbackSize - 2;
  if (reportedLen > 0x1F) {
    reportedLen -= 0x1F;
    // add one null byte per 0xFF bytes of input
    while (reportedLen > 0xFF) {
      ofs.put(0x00);
      reportedLen -= 0xFF;
    }
    
    // write final amount
    ofs.writeu8(reportedLen);
    
    reportedLen = 0;
  }
  
//  int offsetDataPos = ofs.tell();
  cmd = 0x20 + reportedLen;
  
  // write offset data
  int targetOffset = absoluteLookbackDistance - 1;
  TByte offsetByte0 = 0x00;
  TByte offsetByte1 = 0x00;
  offsetByte0 |= ((targetOffset & 0x003F) << 2);
  offsetByte1 |= ((targetOffset & 0x3FC0) >> 6);
  ofs.writeu8(offsetByte0);
  ofs.writeu8(offsetByte1);
  
  // write finalized command byte
  int finalPos = ofs.tell();
  ofs.seek(cmdPos);
  ofs.writeu8(cmd);
  ofs.seek(finalPos);
}

void DandyCmp::writeAbsoluteCommandFirstRun(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen) {
  if (absoluteRunLen <= 0) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::cmp()",
                            "Somehow found no initial absolute run");
  }
  else if ((absoluteRunLen >= 1) && (absoluteRunLen <= 3)) {
    // short copy
    writeInitialShortAbsoluteCmd(ifs, ofs, absoluteRunLen);
  }
  else if (absoluteRunLen <= 0xEE) {
    // special mid-length copy,
    // only possible on first run
    writeInitialMidLenAbsoluteCmd(ifs, ofs, absoluteRunLen);
  }
  else {
    // full command
    writeFullAbsoluteCmd(ifs, ofs, absoluteRunLen);
  }
}

void DandyCmp::writeAbsoluteCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen) {
  if (absoluteRunLen <= 0) {
     // no absolute run (do another lookback)
  }
  else {
    // if absolute run length is 1 to 3,
    // append to the previous lookback command (which must exist,
    // since this is not the first run)
    if (absoluteRunLen <= 3) {
      // set the low 2 bits of the previous cmd to the length
      // of the short absolute run
      ofs.seekoff(-2);
      TByte val = ofs.get();
      val |= absoluteRunLen;
      
      // write updated value
      ofs.seekoff(-1);
      ofs.put(val);
      
      // return to current putpos
      ofs.seekoff(1);
      
      // write absolute data
      for (int i = 0; i < absoluteRunLen; i++) {
        ofs.put(ifs.get());
      }
    }
    else {
      writeFullAbsoluteCmd(ifs, ofs, absoluteRunLen);
    }
  }
}

void DandyCmp::writeFullAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen) {
  if (absoluteRunLen < 4) {
    throw TGenericException(T_SRCANDLINE,
                    "DandyCmp::writeFullAbsoluteCmd()",
                    "Tried to encode impossibly short full absolute run");
  }
  
  int targetOutputPos = ofs.tell();
  // placeholder for cmd
  ofs.put(0x00);
  
  int reportedLen = absoluteRunLen - 3;
  // check for extended size
  if (reportedLen > 0xF) {
    reportedLen -= 0xF;
    // add one null byte per 0xFF bytes of input
    while (reportedLen > 0xFF) {
      ofs.put(0x00);
      reportedLen -= 0xFF;
    }
    
    // write final amount
    ofs.writeu8(reportedLen);
    
    // report zero as final length
    reportedLen = 0;
  }
  
  // write command
  int finalPos = ofs.tell();
  ofs.seek(targetOutputPos);
  ofs.writeu8(0x00 + reportedLen);
  ofs.seek(finalPos);
  
  // copy values
  for (int i = 0; i < absoluteRunLen; i++) ofs.put(ifs.get());
}

void DandyCmp::writeInitialMidLenAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen) {
  int reportedLen = absoluteRunLen + 0x11;
  
  if ((reportedLen < 0x15) || (reportedLen > 0xFF)) {
    throw TGenericException(T_SRCANDLINE,
                  "DandyCmp::writeInitialMidLenAbsoluteCmd()",
                  "Tried to encode impossible initial mid-len absolute");
  }
  
  // write command
  ofs.writeu8(reportedLen);
  
  // copy values
  for (int i = 0; i < absoluteRunLen; i++) ofs.put(ifs.get());
}

void DandyCmp::writeInitialShortAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen) {
  int reportedLen = absoluteRunLen + 0x11;
  
  if ((reportedLen < 0x12) || (reportedLen > 0x14)) {
    throw TGenericException(T_SRCANDLINE,
                  "DandyCmp::writeInitialShortAbsoluteCmd()",
                  "Tried to encode impossible initial short absolute");
  }
  
  // write command
  ofs.writeu8(reportedLen);
  
  // copy values
  for (int i = 0; i < absoluteRunLen; i++) ofs.put(ifs.get());
}

void DandyCmp::decmp(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  std::string checkStr;
  for (int i = 0; i < 4; i++) checkStr += ifs.get();
  
  if (checkStr.compare(std::string("BaB\x00", 4)) != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyCmp::decmp()",
                            "Could not match compressed file signature");
  }
  
  ifs.seekoff(0x4);
  int decompressedSize = ifs.readu32le();
  ifs.seekoff(4);
  int ofsBase = ofs.tell();
  
  // initial run has special rules:
  // - 0x15+ is treated as a long copy.
  //   subtract 0x11 to get length.
  //   count this as a mode1 copy which just occurred and
  //   use the standard follow-up logic.
  // - 0x12-0x14 are a short copy (end of mode2).
  //   subtract 0x11 to get length.
  //   follow up with start of mode2 logic.
  // - 0x0-0x11 are handled as normal mode1
  
  //===============================================
  // sorry for this very ugly code.
  // it's almost exactly how the game does it.
  //===============================================
  
  int cmd = 0;
//  int cmd2 = 0;
  int length = 0;
  int copysrc = 0;
  int temp1 = 0;
  int temp2 = 0;
  
  
start:
  
  //=========================
  // special handling for
  // initial run
  //=========================
  
  // peek next byte
  cmd = ifs.readu8();
  ifs.seekoff(-1);
  
  if (cmd < 0x12) goto mode1;
  
  ifs.get();
  length = cmd - 0x11;
  
  if (length < 0x4) goto shortCopySub;
  
  for (int i = 0; i < length; i++) {
    ofs.put(ifs.get());
  }
  
  goto mode1Lookback;
  
  //==========================================================
  // implicit loop start
  //==========================================================
  
    //=========================
    // absolute copy
    //=========================
    mode1:
//      std::cerr << std::hex << ifs.tell() << " " << ofs.tell() << std::endl;
//      char c;
//      std::cin >> c;
      
      cmd = ifs.readu8();
      if (cmd >= 0x10) goto mode2;
      
      length = cmd;
      if (length == 0) {
        // extended length
        while ((temp1 = ifs.readu8()) == 0) {
          cmd += 0xFF;
        }
        
        length = cmd + 0xF + temp1;
      }
      
      for (int i = 0; i < (length + 3); i++) {
        ofs.put(ifs.get());
      }
    
    //=========================
    // absolute-following
    // lookback copy
    //=========================
      
    mode1Lookback:
      cmd = ifs.readu8();
      
      if (cmd >= 0x10) goto mode2;
      
      temp1 = ifs.readu8();
      copysrc = (ofs.tell() - 0x801) - (cmd >> 2) - (temp1 << 2);
      
      // copy 3 bytes
      for (int i = 0; i < 3; i++) {
        ofs.seek(copysrc + i);
        temp1 = ofs.readu8();
        ofs.seek(ofs.size());
        ofs.put(temp1);
      }
      
      goto shortCopy;
    
    //=========================
    // various lookback modes
    //=========================
      
    mode2:
      if (cmd >= 0x40) {
        temp1 = ifs.readu8();
        copysrc = (ofs.tell() - 1) - ((cmd >> 2) & 0x7) - (temp1 << 3);
        length = (cmd >> 5) - 1;
//        std::cerr << "mode2 0x40: "
//          << std::hex << ifs.tell() << " " << ofs.tell() << " " << copysrc << " " << length << std::endl;
        goto copyLookback;
      }
      else if (cmd >= 0x20) {
        length = (cmd & 0x1F);
        
        if (length == 0) {
          // extended length
          while ((temp1 = ifs.readu8()) == 0) {
            length += 0xFF;
          }
          
          length = length + 0x1F + temp1;
        }
        
        temp1 = ifs.readu8();
        temp2 = ifs.readu8();
        copysrc = (ofs.tell() - 1) - ((temp1 >> 2) + (temp2 << 6));
        
//        std::cerr << "full short lookback: "
//          << std::hex << copysrc <<  " " << length << std::endl;
        
        goto copyLookback;
      }
      else if (cmd >= 0x10) {
        length = (cmd & 0x7);
        
        if (length == 0) {
          // extended length
          while ((temp1 = ifs.readu8()) == 0) {
            length += 0xFF;
          }
          
          length = length + 0x7 + temp1;
        }
        
        temp1 = ifs.readu8();
        temp2 = ifs.readu8();
        copysrc 
          = ofs.tell() - ((cmd & 0x8) << 11) - ((temp1 >> 2) + (temp2 << 6));
        
        if (copysrc == ofs.tell()) goto done;
        
        copysrc -= 0x4000;
        goto copyLookback;
      }
      else /* if (cmd < 0x10) */ {
        
        temp1 = ifs.readu8();
        copysrc = (ofs.tell() - 1) - (cmd >> 2) - (temp1 << 2);
      
//        std::cerr << "mode2 < 0x10: "
//          << std::hex << ifs.tell() << " " << ofs.tell() << " " << cmd << " " << copysrc << " " << length << std::endl;
        
        for (int i = 0; i < 2; i++) {
          ofs.seek(copysrc + i);
          temp1 = ofs.readu8();
          ofs.seek(ofs.size());
          ofs.put(temp1);
        }
        
        goto shortCopy;
      }
    
    //=========================
    // lookback copy
    //=========================
    
    copyLookback:
      for (int i = 0; i < (length + 2); i++) {
        ofs.seek(copysrc + i);
        temp1 = ofs.readu8();
        ofs.seek(ofs.size());
        ofs.put(temp1);
      }
      
      goto shortCopy;
    
    //=========================
    // extra absolute copy
    //=========================
    
    shortCopy:
      
      ifs.seekoff(-2);
      length = ifs.readu8() & 0x03;
      ifs.seekoff(1);
      
//      std::cerr << "shortCopy: "
//        << std::hex << ifs.tell() << " " << ofs.tell() << " " << copysrc << " " << length << std::endl;
      
      if (length == 0) goto mode1;
    
    shortCopySub:
      
      for (int i = 0; i < length; i++) {
        ofs.put(ifs.get());
      }
      
      cmd = ifs.readu8();
//      static_cast<TBufStream&>(ofs).save("test.bin");
      goto mode2;
  
  //==========================================================
  // implicit loop end
  //==========================================================
  
done:
  
  // verify output size
  
  int outputSize = ofs.tell() - ofsBase;
  
/*  std::cout << "header's reported decompressed size: "
    << std::endl
    << "  " << std::hex << decompressedSize << std::endl;
  std::cout << "actual output size: "
    << std::endl
    << "  " << std::hex << outputSize << std::endl; */
  
}

void DandyCmp::cmpLazy(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  int ofsBase = ofs.tell();
  
  int uncompressedSize = ifs.remaining();
  
  // reserve space for header
  ofs.seekoff(0x10);
  
  int dataStart = ofs.tell();
  
    // "compress" data as one long absolute command
    ofs.put(0);
    int reportedLength = uncompressedSize - 0xF - 3;
    while (reportedLength >= (0xFF + 1)) {
      ofs.put(0);
      reportedLength -= 0xFF;
    }
    ofs.put(reportedLength);
    
    // output entire data uncompressed
    while (!ifs.eof()) ofs.put(ifs.get());
    
    // terminator
    ofs.put(0x17);
    ofs.put(0x00);
    ofs.put(0x00);
  
  int dataEnd = ofs.tell();
  
  // header
  ofs.seek(ofsBase);
  ofs.put('B');
  ofs.put('a');
  ofs.put('B');
  ofs.put(0x00);
  ofs.seekoff(4);
//  ofs.writeu32le(dataEnd - dataStart);
  ofs.writeu32le(uncompressedSize);
  ofs.seek(dataEnd);
}

DandyCmp::CmpSearchResult DandyCmp::findBestLookback(
    BlackT::TStream& ifs, StringCacheTable& cache) {
  int startPos = ifs.tell();
  CmpSearchResult result;
  result.address = 0;
  result.length = 0;
  
  // must have at least minimum length sequence available
/*  if (ifs.remaining() < minLookbackSize) return result;
  
  std::list<CmpSearchResult> intermedResults;
  
  // find all lookbacks of the minimum length
  
  int searchStartPos = ifs.tell() - maxLookbackRange;
  if (searchStartPos < 0) searchStartPos = 0;
//  int searchEndPos = startPos - minLookbackSize;
  int searchEndPos = startPos;
  
//  std::cerr << "  " << std::hex << searchStartPos << " " << searchEndPos << std::endl;
  
  // the minimum-length sequence to check against
  std::vector<char> minLengthSequence;
  for (int i = 0; i < minLookbackSize; i++)
    minLengthSequence.push_back(ifs.get());
  
  for (int i = searchStartPos; i < searchEndPos; i++) {
    ifs.seek(i);
    
    bool done = false;
    for (int j = 0; j < minLookbackSize; j++) {
//      if (ifs.get() != minLengthSequence[j]) {
//        done = true;
//        break;
//      }
      char checkChar = 0;
      int targetAddr = i + j;
      if (targetAddr >= startPos) {
        int loopLen = startPos - i;
        ifs.seek(i + (j % loopLen));
        checkChar = ifs.get();
      }
      else {
        checkChar = ifs.get();
      }
      
      if (checkChar != minLengthSequence[j]) {
        done = true;
        break;
      }
    }
    if (done) continue;
    
    CmpSearchResult result;
    result.address = i;
    result.length = minLookbackSize;
    
    intermedResults.push_back(result);
  }
  
//  std::cerr << "x: " << std::hex << result.address << " " << result.length << std::endl;
  
  // fail if no matches
  if (intermedResults.size() == 0) {
    ifs.seek(startPos);
    return result;
  }
  
  // loop, increasing the maximum lookback length by 1 each time.
  // prune all lookbacks that can't be increased to this new maximum.
  // if at any point we attempt to prune the last entry, that is the
  // best possible lookback.
  
//  int maxLookbackSize = ifs.size() - startPos;
//  
//  for (int i = (minLookbackSize + 1); i <= maxLookbackSize; i++) {
////    std::cerr << i << std::endl;
//    bool done = pruneNonMatchingLookbacks(
//        ifs, startPos, intermedResults, i, i - 1);
//    if (done) break;
//  }
//  
//  // should be impossible
//  if (intermedResults.size() == 0) {
//    ifs.seek(startPos);
//    return result;
//  }
//  
//  // return (one of) the final results.
//  // if more than one result remains, they are all of equal length,
//  // but we return the last one (since it has the closest address to the
//  // source and may have a shorter encoding sequence)
//  result = intermedResults.back();
  
//  std::cerr << intermedResults.size() << std::endl;
  
//  CmpSearchResult bestResult = intermedResults.back();
  result = intermedResults.back();
  for (std::list<CmpSearchResult>::iterator it = intermedResults.begin();
       it != intermedResults.end();
       ++it) {
    int matchLen
      = checkMatchingCharLength(
          ifs, startPos, it->address, minLookbackSize);
    // greater than or equals here -- we want the nearest result to the
    // window end, since closer lookbacks may be possible to encode more
    // efficiently
    if (matchLen >= result.length) {
      result.address = it->address;
      result.length = matchLen;
    }
  }
//  std::cerr << "x: " << std::hex << result.address << " " << result.length << std::endl; */

  // i'm a dumbass and a fucking failure
  // this is the most shitty naive approach a fucking first-year would do
  // and i was still too dumb to figure it out
  // seriously, look at that fucking diaster above.
  // and all the related functions below that are now completely useless.
  
/*  if (ifs.remaining() < minLookbackSize) return result;
  
  int searchStartPos = ifs.tell() - maxLookbackRange;
  if (searchStartPos < 0) searchStartPos = 0;
//  int searchEndPos = startPos - minLookbackSize;
  int searchEndPos = startPos;
  
//  for (int i = searchStartPos; i < searchEndPos; i++) {
  for (int i = searchEndPos - 1; i >= searchStartPos; i--) {
    int length = checkMatchingCharLength(ifs, startPos, i, 0);
    if (length >= result.length) {
      result.address = i;
      result.length = length;
      
      if (length >= lookbackSearchCutoffSize) break;
    }
    
//    if (length > 0) {
//      --i;
//      i += length;
//    }
  } */
  
  // VERSION 3:
  // can now compress 512kb of data in 8 seconds instead of 50.
  // hooray.
  // ...and after more tweaking, we're down to 1.4 seconds.
  // good enough.
  // bet i'd feel like an idiot if i could see how the original
  // compressor worked.
  
  if (ifs.remaining() < minLookbackSize) return result;
  if (ifs.remaining() < stringTableCacheEntrySize) return result;
  
  int searchStartPos = ifs.tell() - maxLookbackRange;
  if (searchStartPos < 0) searchStartPos = 0;
  int searchEndPos = startPos;
  
  std::string cacheCode;
  for (int i = 0; i < stringTableCacheEntrySize; i++)
    cacheCode += ifs.get();
  
  StringCacheSubTable& subTable = cache[cacheCode];
  if (subTable.size() > 0) {
    // search in reverse order so we can stop when out of range
    StringCacheSubTable::iterator searchIt = subTable.end();
    --searchIt;
    
    while (true) {
      // stop when we find an entry beyond the start of the search window
//      int checkAddr = searchIt->first;
      int checkAddr = *searchIt;
      if (checkAddr < searchStartPos) break;
/*      if (checkAddr < searchStartPos) {
        // delete everything to the "left" of this position
        while (true) {
          bool last = false;
          if (searchIt == subTable.begin()) last = true;
          
          StringCacheSubTable::iterator deleteIt = searchIt;
          if (!last) --searchIt;
          subTable.erase(deleteIt);
          if (last) break;
        }
        break;
      } */
      
      int matchLen = checkMatchingCharLength(ifs, startPos, checkAddr, 0);
      if (matchLen > result.length) {
        result.address = checkAddr;
        result.length = matchLen;
        
        if (matchLen >= lookbackSearchCutoffSize) break;
      }
      
      if (searchIt == subTable.begin()) break;
      --searchIt;
    }
  }
  
  
  ifs.seek(startPos);
  return result;
}

int DandyCmp::checkMatchingCharLength(BlackT::TStream& ifs,
    int srcStrAddr, int checkStrAddr, int alreadyCheckedChars) {
//  int endPos = ifs.size();
  int maxSrcStrLen = ifs.size() - srcStrAddr;
  
  // size of the loop formed if the string extends past the current input pos
  int loopLen = srcStrAddr - checkStrAddr;
  
  int result = alreadyCheckedChars;
  for (int i = alreadyCheckedChars; i < maxSrcStrLen; i++) {
/*    char checkChar = 0;
    int targetAddr = checkStrAddr + i;
    if (targetAddr >= srcStrAddr) {
      ifs.seek(checkStrAddr + (i % loopLen));
      checkChar = ifs.get();
    }
    else {
      ifs.seek(targetAddr);
      checkChar = ifs.get();
    } */
    
    int targetAddr = checkStrAddr + (i % loopLen);
    ifs.seek(targetAddr);
    char checkChar = ifs.get();
    
    ifs.seek(srcStrAddr + i);
    if (checkChar != ifs.get()) {
      break;
    }
    
    ++result;
  }
  
  return result;
}

bool DandyCmp::pruneNonMatchingLookbacks(
    BlackT::TStream& ifs, int checkSeqAddr,
    std::list<CmpSearchResult>& intermedResults,
    int newMinLookbackSize,
    int oldMinLookbackSize) {
  // return true if done
//  if (intermedResults.size() <= 1) return true;
  
  // read the new check sequence
  ifs.seek(checkSeqAddr);
  std::vector<char> checkSeq;
  for (int i = 0; i < newMinLookbackSize; i++) {
    checkSeq.push_back(ifs.get());
  }
  
  // prune any invalidated results
  std::list<CmpSearchResult>::iterator it = intermedResults.begin();
  while (it != intermedResults.end()) {
    CmpSearchResult& result = *it;
    ifs.seek(result.address);
    
    bool failed = false;
//    for (int i = 0; i < newMinLookbackSize; i++) {
    // we don't need to check the whole string, just the new characters
    for (int i = oldMinLookbackSize; i < newMinLookbackSize; i++) {
      char checkChar = 0;
      int targetAddr = result.address + i;
      
      if (targetAddr >= checkSeqAddr) {
        // special case: if target address is at or beyond the check seq address,
        // imitate loop behavior
        int loopLen = checkSeqAddr - result.address;
        ifs.seek(result.address + (i % loopLen));
        checkChar = ifs.get();
      }
      else {
        // otherwise, next character is simply the next in the sequence
        checkChar = ifs.get();
      }
      
      if (checkChar != checkSeq[i]) {
        failed = true;
        break;
      }
    }
    
    if (failed) {
      // if this is the last remaining entry, leave it: it's the final result
      if (intermedResults.size() == 1) return true;
      
      // remove result
      std::list<CmpSearchResult>::iterator nextIt = it;
      ++nextIt;
      intermedResults.erase(it);
      it = nextIt;
    }
    else {
      // update entry
      result.length = newMinLookbackSize;
      ++it;
    }
  }
  
  return false;
}

DandyCmp::RepSearchResult DandyCmp::findBestRepeat(BlackT::TStream& ifs) {
  int startPos = ifs.tell();
  RepSearchResult result;
  result.seqLen = 0;
  result.numRepeats = 0;
  
  if (ifs.remaining() < 3) return result;
  
  for (int i = 1; i <= maxRepeatSeqLen; i++) {
    int numRepsOfLen = countRepeats(ifs, i);
    if ((numRepsOfLen * i) > (result.seqLen * result.numRepeats)) {
      result.seqLen = i;
      result.numRepeats = numRepsOfLen;
    }
  }
  
  ifs.seek(startPos);
  return result;
}

int DandyCmp::countRepeats(BlackT::TStream& ifs, int seqLen) {
  int startPos = ifs.tell();
  
  if (ifs.remaining() < seqLen) return 0;
  
  std::vector<char> pattern;
  for (int i = 0; i < seqLen; i++) pattern.push_back(ifs.get());
  
  int repCount = 0;
  while (!ifs.eof()) {
    if (ifs.remaining() < seqLen) break;
    
    bool done = false;
    for (int i = 0; i < seqLen; i++) {
      if (ifs.get() != pattern[i]) {
        done = true;
        break;
      }
    }
    
    if (done) break;
    
    ++repCount;
  }
  
  ifs.seek(startPos);
  return repCount;
}


}
