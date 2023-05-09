#include "dandy/DandyImgArc.h"
#include "dandy/DandyCmp.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/MiscMath.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


/*void DandyImgArc::pack(std::string inprefix, int filecount, BlackT::TStream& ofs) {
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
                            "DandyImgArc::pack()",
                            "No files existed to place in PAC");
  }
} */

void DandyImgArc::unpack(BlackT::TStream& ifs, std::string outprefix) {
  // decompress
  TBufStream decompressed;
  DandyCmp::decmp(ifs, decompressed);
  
  decompressed.seek(0);
  unpackDecompressed(decompressed, outprefix);
}

void DandyImgArc::unpackDecompressed(
    BlackT::TStream& ifs, std::string outprefix) {
  int numFiles = ifs.readu32le();
  
  if (numFiles >= 512) {
    throw TGenericException(T_SRCANDLINE,
                            "DandyImgArc::unpackDecompressed()",
                            "Too many files: this is not a real imgarc");
  }
  
  int indexBasePos = ifs.tell();
  
  int currentFilePos = indexBasePos + (numFiles * 4);
  
  for (int i = 0; i < numFiles; i++) {
    // read next file pos
    ifs.seek(indexBasePos + (i * 4));
    int fileSize = ifs.readu32le();
    
    // copy target file
    ifs.seek(currentFilePos);
    TBufStream ofs;
    ofs.writeFrom(ifs, fileSize);
    ofs.save((outprefix
//              + "-"
              + TStringConversion::intToString(i)
              + ".bin").c_str());
    
    // update position of next file
    currentFilePos += fileSize;
  }
}

void DandyImgArc::pack(BlackT::TStream& ofs, int numFiles,
                       std::string inprefix) {
  TBufStream decompressed;
  packDecompressed(decompressed, numFiles, inprefix);
  decompressed.seek(0);
  DandyCmp::cmp(decompressed, ofs);
}

void DandyImgArc::packDecompressed(
    BlackT::TStream& ofs, int numFiles, std::string inprefix) {
  // write number of files
  ofs.writeu32le(numFiles);
  
  int indexBasePos = ofs.tell();
  
  // write placeholders for filesizes
  for (int i = 0; i < numFiles; i++) {
    ofs.writeu32le(0);
  }
  
  for (int i = 0; i < numFiles; i++) {
    // get target file
    std::string filename = inprefix
              + TStringConversion::intToString(i)
              + ".bin";
    TBufStream ifs;
    ifs.open(filename.c_str());
    int rawFileSize = ifs.size();
    
    if (rawFileSize <= 0) {
      throw TGenericException(T_SRCANDLINE,
                              "DandyImgArc::packDecompressed()",
                              std::string("Tried to pack nonexistent file: ")
                                + filename);
    }
    
    int fileStart = ofs.tell();
    
    // copy file content
//    ofs.seek(ofs.size());
    ofs.write((char*)(ifs.data().data()), rawFileSize);
//    while (!ifs.eof()) {
//      ofs.put(ifs.get());
//    }

    // align to word boundary
    ofs.alignToBoundary(4);
    
    int fileEnd = ofs.tell();
    int fileSize = fileEnd - fileStart;
    
    // write size to index
    int endPos = ofs.tell();
    ofs.seek(indexBasePos + (i * 4));
    ofs.writeu32le(fileSize);
    ofs.seek(endPos);
  }
}


}
