#include "dandy/DandyPac.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/MiscMath.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


void DandyPac::pack(std::string inprefix, int filecount, BlackT::TStream& ofs) {
//  int indexByteSize
//    = MiscMath::roundUpToBoundary(filecount * indexEntrySize, sectorSize);
  // actually, it looks like they just used a hardcoded 2-sector index size
  int indexByteSize = sectorSize * 2;

  int ofsBase = ofs.tell();
  int indexPutPos = 0;
  int filePutPos = indexByteSize;

  bool aFileExisted = false;
  for (int i = 0; i < filecount; i++) {
    std::string filename = inprefix
                            + TStringConversion::intToString(i)
                            + ".bin";
    
    if (!TFileManip::fileExists(filename)) {
      // blank entry in index
      ofs.seek(indexPutPos);
      ofs.writeu32le(0);
      ofs.writeu32le(0);
      indexPutPos += indexEntrySize;
      continue;
    }
    else aFileExisted = true;
    
    TBufStream ifs;
    ifs.open(filename.c_str());
    
    int sectorNum = filePutPos / sectorSize;
    int fileSize = ifs.size();
    int fileSectorSize
      = MiscMath::roundUpToBoundary(fileSize, sectorSize);
    int padSize = fileSectorSize - fileSize;
    
    // write index entry
    ofs.seek(ofsBase + indexPutPos);
    ofs.writeu32le(sectorNum);
    ofs.writeu32le(fileSize);
    indexPutPos += 8;
    
    // write file data
    ofs.seek(ofsBase + filePutPos);
//    ofs.writeFrom(ifs, fileSize);
    for (int i = 0; i < fileSize; i++) ofs.put(ifs.get());
    for (int i = 0; i < padSize; i++) ofs.put(0x00);
    filePutPos += fileSectorSize;
  }
  
  if (!aFileExisted) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyPac::pack()",
                            "No files existed to place in PAC");
  }
}

void DandyPac::unpack(BlackT::TStream& ifs, std::string outprefix) {
  int basePos = ifs.tell();
  int entryNum = 0;
  
  int firstEntrySectorNum = 0;
  if (ifs.remaining() >= 8) {
    firstEntrySectorNum = ifs.readu32le();
  }
  int firstEntryByteNum = firstEntrySectorNum * sectorSize;
  
  while ((basePos + (entryNum * 8)) < firstEntryByteNum) {
    ifs.seek(basePos + (entryNum * 8));
    int sectorNum = ifs.readu32le();
    int fileSize = ifs.readu32le();
    
//    if ((sectorNum == 0) && (fileSize == 0)) break;
    if ((sectorNum == 0) && (fileSize == 0)) {
      ++entryNum;
      continue;
    }
    
    ifs.seek(basePos + (sectorNum * sectorSize));
    
    TBufStream ofs;
    ofs.writeFrom(ifs, fileSize);
    ofs.save((outprefix
//              + "-"
              + TStringConversion::intToString(entryNum)
              + ".bin").c_str());
    
    ++entryNum;
  }
}


}
