#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxBitmap.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;
//using namespace Discaster;
using namespace Psx;

string as3bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 3) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

PsxBitmap::Format getFormatOfNumber(int num) {
  switch (num) {
  case 16:
    return PsxBitmap::format_rgb;
    break;
  case 2:
    return PsxBitmap::format_2bit;
    break;
  case 4:
    return PsxBitmap::format_4bit;
    break;
  case 8:
    return PsxBitmap::format_8bit;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "getFormatOfNumber()",
                            std::string("Unknown format code: ")
                            + TStringConversion::intToString(num));
    break;
  }
}

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "PlayStation image colorizer from palette" << endl;
    cout << "Usage: " << argv[0] << " [inimg] [palette] [outimg] [bpp]" << endl;
    cout << "Options:" << endl;
    cout << "   -paloffset     Bytes to skip in palette file" << endl;
    cout << "   -crop [x] [y] [w] [h]   Crop to selection" << endl;
    cout << "   --flippalend   Flip palette endianness" << endl;
    cout << "Input image must be grayscale." << endl;
    cout << "Supported widths are 2, 4, and 8." << endl;
    
    return 0;
  }
  
  string inFile(argv[1]);
  string inPalette(argv[2]);
  string outFile(argv[3]);
  int bpp = TStringConversion::stringToInt(std::string(argv[4]));
  
  PsxBitmap::Format format = getFormatOfNumber(bpp);
  PsxBitmap img;
  
  int palOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-paloffset", &palOffset);
//  palOffset *= 2;
  
  bool hasCrop = false;
  TRect cropBox;
  int cropOpt = TOpt::findOpt(argc, argv, "-crop");
  if (cropOpt != -1) {
    cropBox.setX(TStringConversion::stringToInt(std::string(argv[cropOpt + 1])));
    cropBox.setY(TStringConversion::stringToInt(std::string(argv[cropOpt + 2])));
    cropBox.setW(TStringConversion::stringToInt(std::string(argv[cropOpt + 3])));
    cropBox.setH(TStringConversion::stringToInt(std::string(argv[cropOpt + 4])));
    hasCrop = true;
  }
  
  bool flipPalEnd = false;
  if (TOpt::hasFlag(argc, argv, "--flippalend")) {
    flipPalEnd = true;
  }
  
  // read image
  img.setFormat(format);
  {
    TGraphic grp;
    TPngConversion::RGBAPngToGraphic(inFile, grp);
    
    if (hasCrop) {
      TGraphic copy = grp;
      grp = TGraphic(cropBox.w(), cropBox.h());
      grp.copy(copy, TRect(0, 0, 0, 0), cropBox);
    }
    
    img.fromGrayscaleGraphic(grp);
  }
  
  // oops i forget which header pow() is in, whatever
  int numColors = 1;
  for (int i = 0; i < bpp; i++) numColors *= 2;
  
  PsxPalette palette;
  {
    TBufStream palIfs;
    palIfs.open(inPalette.c_str());
    palIfs.seekoff(palOffset);
    
    TBufStream palOfs;
    for (int i = 0; i < numColors; i++) {
      int next;
      
      if (flipPalEnd)
        next = palIfs.readu16be();
      else
        next = palIfs.readu16le();
        
      palOfs.writeu16le(next);
    }
    
    palOfs.seek(0);
    palette.read(palOfs, numColors);
  }
  
  img.setPalette(palette);
  
  // convert to raw data
//  img.setFormat(outFormat);
/*  TBufStream raw;
  img.writePixelData(raw);
  raw.seek(0);
  
  // read back in target format
  PsxBitmap convertedImg;
  // ratio of old image width to new
  double scaleFactor = (double)inWidth / (double)outWidth;
  convertedImg.readPixelData(raw, outFormat,
                             (double)img.w() * scaleFactor, img.h()); */
  
  // save as graphic
  {
/*    TGraphic grp;
    if (outFormat == PsxBitmap::format_rgb) {
      convertedImg.toGraphic(grp, true, false);
    }
    else {
      convertedImg.toGrayscaleGraphic(grp);
    }
    TPngConversion::graphicToRGBAPng(outFile, grp); */
    
    TGraphic grp;
    img.toGraphic(grp, true, false);
    TPngConversion::graphicToRGBAPng(outFile, grp);
  }
  
  return 0;
}
