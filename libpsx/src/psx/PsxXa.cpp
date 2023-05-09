#include "psx/PsxXa.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/MiscMath.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Psx {


const int PsxXa::pos_xa_adpcm_table[5]
  = {0, 60, 115, 98, 122};
const int PsxXa::neg_xa_adpcm_table[5]
  = {0, 0, -52, -55, -60};

void PsxXa::decodeXa(BlackT::TStream& ifs, TSoundFile& dst,
                     int maxSectors) {
  // read subheader to get parameters
  ifs.seekoff(0x10);
  int initialFileNum = ifs.readu8();
  int initialChannelNum = ifs.readu8();
  int initialSubmode = ifs.readu8();
  int initialCodinginfo = ifs.readu8();
  ifs.seekoff(-0x14);
  
  StereoMode stereoMode;
  {
    int rawStereoMode = initialCodinginfo & 0x03;
    switch (rawStereoMode) {
    case 0:
      stereoMode = stereoMode_mono;
      dst.setChannels(1);
      break;
    case 1:
      stereoMode = stereoMode_stereo;
      dst.setChannels(2);
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "PsxXa::decodeXa()",
                              TString("Invalid stereo mode: ")
                              + TStringConversion::intToString(rawStereoMode));
      break;
    }
  }
  
  BitRate bitRate;
  {
    int rawBitRate = (initialCodinginfo & 0x0C) >> 2;
    switch (rawBitRate) {
    case 0:
      bitRate = bitRate_37800;
      dst.setRate(37800);
      break;
    case 1:
      bitRate = bitRate_18900;
      dst.setRate(18900);
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "PsxXa::decodeXa()",
                              TString("Invalid bit rate: ")
                              + TStringConversion::intToString(rawBitRate));
      break;
    }
  }
  
  BitWidth bitWidth;
//  int numBlocks;
  {
    int rawBitWidth = (initialCodinginfo & 0x30) >> 4;
    switch (rawBitWidth) {
    case 0:
      bitWidth = bitWidth_4;
//      numBlocks = 8;
      break;
    case 1:
      bitWidth = bitWidth_8;
//      numBlocks = 4;
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "PsxXa::decodeXa()",
                              TString("Invalid bit width: ")
                              + TStringConversion::intToString(rawBitWidth));
      break;
    }
  }
  
//  if (stereoMode == stereoMode_stereo) numBlocks /= 2;
  
  // decide interleave size (number of sectors between successive units
  // of this stream) based on above parameters
//  int interleaveSize = 8;
//  if (stereoMode == stereoMode_mono) interleaveSize *= 2;
//  if (bitRate == bitRate_18900) interleaveSize *= 2;
  int interleaveSize = getInterleaveSize(stereoMode, bitRate);
  
//  std::cerr << interleaveSize << std::endl;
  
  // if mono, only left is used
  int oldLeft, olderLeft, oldRight, olderRight = 0;
  int sectorCount = 0;
  while (true) {
    if (ifs.eof()) {
      // can't do this -- e.g. video files may only terminate
      // the video stream
//      throw TGenericException(T_SRCANDLINE,
//                              "PsxXa::decodeXa()",
//                              TString("Unexpected end of file"));
      break;
    }
    
    int sectorBasePos = ifs.tell();
//    std::cerr << std::hex << sectorBasePos << std::endl;
    
    // read in next sector
    TArray<TByte> sectorData(bytesPerRawSector);
    ifs.read((char*)sectorData.data(), bytesPerRawSector);
    
    int fileNum = sectorData[0x10];
    int channelNum = sectorData[0x11];
    if ((fileNum != initialFileNum)
        || (channelNum != initialChannelNum)) {
      break;
    }
    
    for (int i = 0; i < 0x12; i++) {
      TByte* src = sectorData.data() + 24 + (i * 128);
      
      OutputList outputLeft;
      OutputList outputRight;
      
      if (bitWidth == bitWidth_4) {
        for (int j = 0; j < 4; j++) {
          
          if (stereoMode == stereoMode_mono) {
            decode4bit(src, outputLeft, j, 0,
                       oldLeft, olderLeft);
            decode4bit(src, outputLeft, j, 1,
                       oldLeft, olderLeft);
          }
          else {
            decode4bit(src, outputLeft, j, 0,
                       oldLeft, olderLeft);
            decode4bit(src, outputRight, j, 1,
                       oldRight, olderRight);
          }
        }
      }
      else {
        // TODO
        throw TGenericException(T_SRCANDLINE,
                                "PsxXa::decodeXa()",
                                TString("8-bit XA not supported"));
      }
      
      while (!outputLeft.empty() || !outputRight.empty()) {
        if (!outputLeft.empty()) {
          dst.addSampleS16(outputLeft.front());
          outputLeft.pop_front();
        }
        
        if (!outputRight.empty()) {
          dst.addSampleS16(outputRight.front());
          outputRight.pop_front();
        }
      }
    }
    
    // check if sector limit reached
    ++sectorCount;
    if ((maxSectors != -1) && (sectorCount >= maxSectors)) break;
    
    // check for EOF marker
    if (((sectorData[0x12] & 0x80) != 0)) {
//      std::cerr << "eof: " << sectorBasePos << std::endl;
      break;
    }
    
    // seek to next sector
    ifs.seek(sectorBasePos + (interleaveSize * bytesPerRawSector));
  }
}

void PsxXa::decode4bit(TByte* src, OutputList& dst,
    int block, int nybble,
    int& old, int& older) {
//  shift  = 12 - (src[4+blk*2+nibble] AND 0Fh)
//  filter =      (src[4+blk*2+nibble] AND 30h) SHR 4
//  f0 = pos_xa_adpcm_table[filter]
//  f1 = neg_xa_adpcm_table[filter]
//  for j=0 to 27
//    t = signed4bit((src[16+blk+j*4] SHR (nibble*4)) AND 0Fh)
//    s = (t SHL shift) + ((old*f0 + older*f1+32)/64);
//    s = MinMax(s,-8000h,+7FFFh)
//    halfword[dst]=s, dst=dst+2, older=old, old=s
//  next j

  int headerOffset = 4 + (block * 2) + nybble;

  int shift = 12 - (src[headerOffset] & 0x0F);
  int filter = (src[headerOffset] & 0x30) >> 4;
  int f0 = pos_xa_adpcm_table[filter];
  int f1 = neg_xa_adpcm_table[filter];
  
  for (int i = 0; i < 28; i++) {
    int t = (src[16 + block + (i * 4)] >> (nybble * 4)) & 0x0F;
    // apply nybble signedness
    if (t >= 8) t -= 16;
    
    int s = (t << shift) + (((old * f0) + (older * f1) + 32) / 64);
    MiscMath::clamp(s, -0x8000, 0x7FFF);
    
    dst.push_back(s);
    older = old;
    old = s;
  }
}

int PsxXa::getInterleaveSize(StereoMode stereoMode, BitRate bitRate) {
  int interleaveSize = 8;
  if (stereoMode == stereoMode_mono) interleaveSize *= 2;
  if (bitRate == bitRate_18900) interleaveSize *= 2;
  return interleaveSize;
}


}
