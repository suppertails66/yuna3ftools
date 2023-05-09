#ifndef DANDYCMP_H
#define DANDYCMP_H


#include "util/TStream.h"
#include <string>
#include <list>
#include <map>
#include <unordered_map>

namespace Psx {


class DandyCmp {
public:
  
  static void cmp(BlackT::TStream& ifs, BlackT::TStream& ofs);
  static void decmp(BlackT::TStream& ifs, BlackT::TStream& ofs);
  // old "compression" routine that just shoves everything into one
  // long absolute command, resulting in a net gain in filesize
  static void cmpLazy(BlackT::TStream& ifs, BlackT::TStream& ofs);
  
protected:
  
  struct CmpSearchResult {
    int address;
    int length;
  };
  
  struct RepSearchResult {
    int seqLen;
    int numRepeats;
  };
  
//  const static int maxLookbackRange = 0x87FF;
  const static int maxLookbackRange = 0xBFFF;
  // arbitrarily chosen value -- the format actually supports
  // unlimited lengths, but lower lengths to check naturally
  // speed up compression.
  // it would probably be reasonable to, in the event that we
  // actually find a lookback of this maximum length, have a special
  // case where we just copy it out as far as we can.
//  const static int maxLookbackSize = 0x200;
  const static int minLookbackSize = 4;
  // FIXME: it's more complicated than this.
  // different commands have differing lengths,
  // and i haven't implemented all of them because
  // i'm hoping i won't have to.
  // but for the current implementation, 4 is correct.
  const static int minEfficientLookbackSize = 4;
  
  // if minRepeatEfficiency is nonzero, upcoming looped sequences of
  // up to maxRepeatSeqLen bytes in size, which extend for a total of at least
  // minRepeatEfficiency bytes, will be prioritized over searching
  // the window for matching lookbacks.
  // results in faster but worse compression.
//  const static int minRepeatEfficiency = 5;
  const static int minRepeatEfficiency = 6;
  // ex. a value of 3 will check for "01 01 01...", "01 02 01 02...", and
  // "01 02 03 01 02 03..."
  const static int maxRepeatSeqLen = 2;
  
//  typedef std::map<int, int> StringCacheSubTable;
  typedef std::list<int> StringCacheSubTable;
  typedef std::unordered_map<std::string, StringCacheSubTable> StringCacheTable;
  // size of the strings placed in the cache.
  // also ends up being the implicit minimum size of a lookback.
  // higher = faster, but worse compression
  const static int stringTableCacheEntrySize = 3;
  // empty the cache whenever it has this many tables in it.
  // lower = faster but with massive reduction in compression level
  const static int autoCacheEmptyThreshold = 0;
  // remove unused tables/entries from cache every time we go through this
  // many bytes in the input.
  // surprisingly, this seems to actually yield a fractional speedup...?
//  const static int autoCacheCleanThreshold = 0xC000;
  const static int autoCacheCleanThreshold = 0xC000;
  // HACK: if we see a lookback this long or longer, assume we can't do better
  // and stop searching there.
  // lower = faster, but worse compression
  const static int lookbackSearchCutoffSize = 20;
  
  static void addCacheEntry(BlackT::TStream& ifs,
                            StringCacheTable& cache);
  static void cleanCache(StringCacheTable& cache, int currentPos);
  
  static CmpSearchResult findBestLookback(BlackT::TStream& ifs,
                                          StringCacheTable& cache);
  static bool pruneNonMatchingLookbacks(
      BlackT::TStream& ifs, int checkSeqAddr,
      std::list<CmpSearchResult>& intermedResults,
      int newMinLookbackSize,
      int oldMinLookbackSize);
  static int checkMatchingCharLength(BlackT::TStream& ifs,
      int stringStartPos, int srcAddr, int alreadyCheckedChars);
  
  static RepSearchResult findBestRepeat(BlackT::TStream& ifs);
  static int countRepeats(BlackT::TStream& ifs, int seqLen);
  
  static void writeAbsoluteCommandFirstRun(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen);
  static void writeAbsoluteCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen);
    
  static void writeFullAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen);
  static void writeInitialMidLenAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen);
  static void writeInitialShortAbsoluteCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteRunLen);
  
  static void writeLookbackCommand(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize);
    
  static void writeMiniShortLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize);
  static void writeFullShortLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize);
  static void writeLongLookbackCmd(BlackT::TStream& ifs,
    BlackT::TStream& ofs, int absoluteLookbackDistance,
    int lookbackSize);
  
};


}


#endif
