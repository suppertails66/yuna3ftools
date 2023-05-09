#ifndef PSXXA_H
#define PSXXA_H


#include "util/TStream.h"
#include "util/TByte.h"
#include "util/TSoundFile.h"
#include <list>

namespace Psx {


class PsxXa {
public:
  static void decodeXa(BlackT::TStream& ifs, BlackT::TSoundFile& dst,
                       int maxSectors = -1);
protected:
  const static int bytesPerRawSector = 0x930;
  const static int pos_xa_adpcm_table[];
  const static int neg_xa_adpcm_table[];
  
  typedef std::list<int> OutputList;
  
  enum StereoMode {
    stereoMode_none,
    stereoMode_mono,
    stereoMode_stereo
  };
  
  enum BitRate {
    bitRate_none,
    bitRate_37800,
    bitRate_18900
  };
  
  enum BitWidth {
    bitWidth_none,
    bitWidth_4,
    bitWidth_8
  };
  
  static void decode4bit(BlackT::TByte* src, OutputList& dst,
    int block, int nybble,
    int& old, int& older);
  
  static int getInterleaveSize(StereoMode stereoMode, BitRate bitRate);
};


}


#endif
