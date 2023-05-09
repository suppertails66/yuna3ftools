#include "pom/PomCmp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include "exception/TGenericException.h"
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {

PomCmp::StringCacheTable PomCmp::currentCache = StringCacheTable();
int PomCmp::lastCacheClean = 0;
int PomCmp::currentCommandWord = 0;
int PomCmp::currentCommandWordWritePos = 0;
int PomCmp::currentCommandWordBitNum = 0;

void PomCmp::decmpRux(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  std::string checkStr;
  for (int i = 0; i < 3; i++) checkStr += ifs.get();
  
  if (checkStr.compare(std::string("RUX", 3)) != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "PomCmp::decmpRux()",
                            "Could not match RUX file signature");
  }
  
//  ifs.seekoff(0x3);
  int flags = ifs.readu8();
  
  if (flags != 0) {
    std::cout << "WARNING: flags is " << flags << std::endl;
  }
  
  int decompressedSize = ifs.readu32le();
//  ifs.seekoff(4);
  int ofsBase = ofs.tell();
  
  int cmdBitsLeft = 0;
  int cmd = 0;
  while (true) {
    if ((ofs.tell() - ofsBase) >= decompressedSize) break;
    
    if (cmdBitsLeft == 0) {
      cmd = ifs.readu16le();
      cmdBitsLeft = 0x10;
    }
    
    if (cmd & 0x8000) {
      // lookback
      int subcmd = ifs.readu16le();
      int topnyb = (subcmd >> 12) & 0xF;
      
      int length;
      int distance = subcmd & 0xFFF;
      if (topnyb != 0) {
        length = topnyb;
      }
      else {
        length = ifs.readu8() + 0x10;
      }
      
      ++length;
      ++distance;
      
//      std::cerr << std::hex << ifs.tell() << " " << distance << " " << length << std::endl;
      
      int pos = ofs.tell();
      for (int i = 0; i < length; i++) {
        ofs.seekoff(-distance);
        char next = ofs.get();
        ofs.seek(pos + i);
        ofs.put(next);
      }
    }
    else {
      // literal
      ofs.put(ifs.get());
    }
    
    --cmdBitsLeft;
    cmd = (cmd << 1) & 0xFFFF;
  }
  
  // verify output size
  
//  int outputSize = ofs.tell() - ofsBase;
  
/*  std::cout << "header's reported decompressed size: "
    << std::endl
    << "  " << std::hex << decompressedSize << std::endl;
  std::cout << "actual output size: "
    << std::endl
    << "  " << std::hex << outputSize << std::endl; */
  
}

void PomCmp::cmpRux(BlackT::TStream& ifs, BlackT::TStream& ofs) {
  int ofsBase = ofs.tell();
  
  int uncompressedSize = ifs.remaining();
  
  // reserve space for header
  int headerStart = ofs.tell();
  ofs.seekoff(0x8);
  
  int dataStart = ofs.tell();
  
//  StringCacheTable currentCache;
//  int lastCacheClean = 0;
//  int currentCommandWord = 0;
//  int currentCommandWordWritePos = 0;
//  int currentCommandWordBitNum = bitsPerCommandWord - 1;
  
  currentCache = StringCacheTable();
  lastCacheClean = 0;
  currentCommandWord = 0;
  currentCommandWordBitNum = 0;
  
  // init first command word
  currentCommandWordWritePos = ofs.tell();
  ofs.writeu16le(0);
  
  while (!ifs.eof()) {
    if (autoCacheEmptyThreshold != 0) {
      if (currentCache.size() >= autoCacheEmptyThreshold) currentCache.clear();
    }
    
    if (autoCacheCleanThreshold != 0) {
      if ((ifs.tell() - lastCacheClean) >= autoCacheCleanThreshold) {
        cleanCache(currentCache, ifs.tell());
        lastCacheClean = ifs.tell();
      }
    }
    
    // cache this location
    addCacheEntry(ifs, currentCache);
    
    // find best lookback
    CmpSearchResult bestLookback;
    bestLookback.address = 0;
    bestLookback.length = 0;
    bestLookback = findBestLookback(ifs, currentCache);
    
    // very lazily ascertain whether it would be more efficient
    // to ignore a lookback at this position and just do an absolute
    // command instead
    CmpSearchResult bestLookbackNext;
    bestLookbackNext.address = 0;
    bestLookbackNext.length = 0;
    if (doLookbackIgnoreCheck) {
      if (!ifs.eof()) {
        ifs.seekoff(1);
        bestLookbackNext = findBestLookback(ifs, currentCache);
        ifs.seekoff(-1);
      }
    }
    
//    if ((bestLookbackNext.length > bestLookback.length)) {
//      writeAbsoluteCommand(ifs, ofs);
//      bestLookback = bestLookbackNext;
//      addCacheEntry(ifs, currentCache);
//    }
    
    // if lookback is efficient, add it
    if ((bestLookback.length >= minEfficientLookbackSize)
        && (bestLookbackNext.length <= bestLookback.length)
        ) {
//      std::cerr << "lookback: " << std::hex << ifs.tell() << " " << ofs.tell() << " " << currentCommandWord << " " << currentCommandWordBitNum << std::endl;
//      char c;
//      std::cin >> c;
      
      int absoluteLookbackDistance = ifs.tell() - bestLookback.address;
      
      // cap length to maximum possible
      int trueLen = bestLookback.length;
      if (trueLen > maxLookbackLength) trueLen = maxLookbackLength;
      
      writeLookbackCommand(ifs, ofs, absoluteLookbackDistance, trueLen);
    }
    // otherwise, add an absolute command
    else {
      // repeat check
      if ((minRepeatEfficiency > 0)) {
        RepSearchResult bestRepeat = findBestRepeat(ifs);
        int repeatEfficiency = bestRepeat.seqLen * bestRepeat.numRepeats;
        if (repeatEfficiency >= minRepeatEfficiency) {
  //        std::cerr << "repeat: " << std::hex << ifs.tell() << " " << ofs.tell() << " " << currentCommandWord << " " << currentCommandWordBitNum << std::endl;
  //        char c;
  //        std::cin >> c;
          
          // cap to max length
          if (repeatEfficiency > maxLookbackLength)
            repeatEfficiency = maxLookbackLength;
          
          // create absolute command with the target characters
          for (int i = 0; i < bestRepeat.seqLen; i++) {
            writeAbsoluteCommand(ifs, ofs);
          }
          // create lookback covering those characters as many times as needed
          writeLookbackCommand(ifs, ofs, bestRepeat.seqLen, repeatEfficiency);
          
          // skip content covered by the lookback
          //ifs.seekoff(lookbackSize);
  //        for (int i = 0; i < repeatEfficiency; i++) {
  //          addCacheEntry(ifs, currentCache);
  //          ifs.seekoff(1);
  //        }
          
          continue;
        }
      }
      
//      std::cerr << "absolute: " << std::hex << ifs.tell() << " " << ofs.tell() << " " << currentCommandWord << " " << currentCommandWordBitNum << std::endl;
//      char c;
//      std::cin >> c;
      writeAbsoluteCommand(ifs, ofs);
    }
  }
  
  // write final command word if not empty
  if (currentCommandWordBitNum > 0) {
    // write command word
    int nextpos = ofs.tell();
    ofs.seek(currentCommandWordWritePos);
    ofs.writeu16le(currentCommandWord);
    ofs.seek(nextpos);
  }
  
  // write header
  int endpos = ofs.tell();
  ofs.seek(headerStart);
  ofs.writeString(std::string("RUX"));
  // TODO: these are flags?
  ofs.put(0x00);
  ofs.writeu32le(uncompressedSize);
  ofs.seek(endpos);
}

void PomCmp::advanceCommandWordBit(BlackT::TStream& ofs) {
  // check if new command word needed
  ++currentCommandWordBitNum;
  if (currentCommandWordBitNum >= bitsPerCommandWord) {
    // write command word
    int nextpos = ofs.tell();
    ofs.seek(currentCommandWordWritePos);
    ofs.writeu16le(currentCommandWord);
    ofs.seek(nextpos);
    
    // write placeholder for command word in output
    currentCommandWordWritePos = ofs.tell();
    ofs.writeu16le(0);
    
    // reset fields
    currentCommandWord = 0;
    currentCommandWordBitNum = 0;
  }
}

void PomCmp::writeAbsoluteCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs) {
  ofs.put(ifs.get());
  
  // leave command bit clear
  
  advanceCommandWordBit(ofs);
}

void PomCmp::writeLookbackCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize) {
  if (absoluteLookbackDistance > maxLookbackRange) {
    throw TGenericException(T_SRCANDLINE,
                            "PomCmp::writeLookbackCommand()",
                            std::string("Unencodable lookback distance: ")
                              + TStringConversion
                                ::intToString(absoluteLookbackDistance));
  }
  
  if ((lookbackSize <= 0) || (lookbackSize > maxLookbackLength)) {
    throw TGenericException(T_SRCANDLINE,
                            "PomCmp::writeLookbackCommand()",
                            std::string("Unencodable lookback size: ")
                              + TStringConversion::intToString(lookbackSize));
  }
  
  int output = 0;
  
  output |= (absoluteLookbackDistance - 1);
  
  // top nybble is zero if distance >= 0x10 (= 8-bit size);
  // otherwise, it's (size - 1)
  bool sizeIs8Bit = false;
  if (lookbackSize <= maxLookbackShortSize) {
    output |= ((lookbackSize - 1) << 12);
  }
  else {
    // 8-bit size
    sizeIs8Bit = true;
  }
  
  // write encoded lookback
  ofs.writeu16le(output);
  
  // write 8-bit size if present
  if (sizeIs8Bit)
    ofs.writeu8(lookbackSize - 0x11);
  
  // set command bit
  currentCommandWord
    |= (1 << (bitsPerCommandWord - currentCommandWordBitNum - 1));
  
  advanceCommandWordBit(ofs);
  
  // advance ifs
  for (int i = 0; i < lookbackSize; i++) {
    addCacheEntry(ifs, currentCache);
    ifs.seekoff(1);
  }
}

void PomCmp::addCacheEntry(BlackT::TStream& ifs,
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

void PomCmp::cleanCache(StringCacheTable& cache, int currentPos) {
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

PomCmp::CmpSearchResult PomCmp::findBestLookback(
    BlackT::TStream& ifs, StringCacheTable& cache) {
  int startPos = ifs.tell();
  CmpSearchResult result;
  result.address = 0;
  result.length = 0;
  
  // VERSION 3:
  // can now compress 512kb of data in 8 seconds instead of 50.
  // hooray.
  // ...and after more tweaking, we're down to 1.4 seconds.
  // good enough.
  // bet i'd feel like an idiot if i could see how the original
  // compressor worked.
  
//  if (ifs.remaining() < minLookbackSize) return result;
  if (ifs.remaining() < minEfficientLookbackSize) return result;
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
        
        if ((lookbackSearchCutoffSize > 0)
            && matchLen >= lookbackSearchCutoffSize) break;
      }
      
      if (searchIt == subTable.begin()) break;
      --searchIt;
    }
  }
  
  
  ifs.seek(startPos);
  return result;
}

int PomCmp::checkMatchingCharLength(BlackT::TStream& ifs,
    int srcStrAddr, int checkStrAddr, int alreadyCheckedChars) {
//  int endPos = ifs.size();
  int maxSrcStrLen = ifs.size() - srcStrAddr;
  
  // FIXME: does this make sense?
  if (maxLookbackLength < maxSrcStrLen) maxSrcStrLen = maxLookbackLength;
  
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

/*bool PomCmp::pruneNonMatchingLookbacks(
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
} */

PomCmp::RepSearchResult PomCmp::findBestRepeat(BlackT::TStream& ifs) {
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

int PomCmp::countRepeats(BlackT::TStream& ifs, int seqLen) {
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
