#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TIniFile.h"
#include "util/TBufStream.h"
#include "util/TOfstream.h"
#include "util/TIfstream.h"
#include "util/TStringConversion.h"
#include "util/TBitmapFont.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;
using namespace BlackT;

const static int charW = 16;
const static int charH = 16;
const static int charSize = (charW * charH / 2);

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yuna 3 FE font extractor" << endl;
    cout << "Usage: " << argv[0] << " <infont>"
      << " <outfont>"
      << endl;
    
    return 0;
  }
  
  string inFontName(argv[1]);
  string outFontName(argv[2]);
  
  TBufStream ifs;
  ifs.open(inFontName.c_str());
  
  std::vector<TGraphic> chars;
  int numChars = ifs.size() / charSize;
  for (int i = 0; i < numChars; i++) {
    TGraphic grp(charW, charH);
    
    for (int j = 0; j < charH; j++) {
      for (int i = 0; i < (charW / 4); i++) {
        int x = (i * 4);
        int y = j;
        
        int next = ifs.readu16be();
        int value1 = (next & 0xF000) >> 12;
        int value2 = (next & 0x0F00) >> 8;
        int value3 = (next & 0x00F0) >> 4;
        int value4 = (next & 0x000F) >> 0;
        
        value1 = (value1 | (value1 << 4));
        value2 = (value2 | (value2 << 4));
        value3 = (value3 | (value3 << 4));
        value4 = (value4 | (value4 << 4));
        
        grp.setPixel(x + 0, y, TColor(value1, value1, value1));
        grp.setPixel(x + 1, y, TColor(value2, value2, value2));
        grp.setPixel(x + 2, y, TColor(value3, value3, value3));
        grp.setPixel(x + 3, y, TColor(value4, value4, value4));
      }
    }
    
    chars.push_back(grp);
  }
  
  TGraphic output(charW * 16, ((chars.size() + 0xF) / 16) * charH);
  for (int i = 0; i < chars.size(); i++) {
    int x = (i % 16) * charW;
    int y = (i / 16) * charH;
    output.copy(chars[i],
                TRect(x, y, 0, 0));
  }
  
  TPngConversion::graphicToRGBAPng(outFontName, output);
  
  return 0;
}
