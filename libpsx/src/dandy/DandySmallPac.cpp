#include "dandy/DandySmallPac.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace BlackT;

namespace Psx {

  
void DandySmallPac::unpack(BlackT::TStream& ifs, std::string outprefix) {
  int basePos = ifs.tell();
  int numFiles = ifs.readu32le();
  int indexBase = ifs.tell();
  
  int entryNum;
  for (entryNum = 0; entryNum < numFiles; entryNum++) {
    ifs.seek(indexBase + (entryNum * 4));
    int offset = ifs.readu32le();
    
    int fileEnd;
    if (entryNum == (numFiles - 1)) fileEnd = ifs.size();
    else fileEnd = ifs.readu32le();
    
    int fileSize = fileEnd - offset;
    
    ifs.seek(basePos + offset);
    
    TBufStream ofs;
    ofs.writeFrom(ifs, fileSize);
    ofs.save((outprefix
//              + "-"
              + TStringConversion::intToString(entryNum)
              + ".bin").c_str());
  }
}


}
