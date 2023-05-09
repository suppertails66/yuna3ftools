#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "util/TGraphic.h"
#include "util/TColor.h"
#include "util/TPngConversion.h"
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
//#include "dandy/DandyPac.h"
//#include "dandy/DandyScriptScanner.h"
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;
//using namespace Discaster;
//using namespace Psx;

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

string as1bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 1) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

const static unsigned int exeBase = 0x80110000;
const static unsigned int heapStart = 0x10000;
const static unsigned int heapEnd = 0x108000;
const static unsigned int pointerBase = 0x80000000;

int main(int argc, char* argv[]) {
/*  TBufStream ifs;
  ifs.open("base/SLPS_014.51_noheader");
  
  int tableBasePos = 0x801AF468 - exeBase;
  
  for (int i = 0; i < 349; i++) {
    ifs.seek(tableBasePos + (i * 12));
    unsigned int namePtr = ifs.readu32le();
    unsigned int sectorCount = ifs.readu32le();
    unsigned int sectorNum = ifs.readu32le();
    
    ifs.seek(namePtr - exeBase);
    std::string name;
    ifs.readCstrString(name);
    
    std::cout << name << " " << sectorCount << " " << sectorNum << std::endl;
  } */
  
/*  if (argc < 2) return 0;
  
  TBufStream ifs;
  ifs.open(argv[1]);
  
  ifs.seek(heapStart);
  int num = 0;
  while (true) {
    int pos = ifs.tell();
    
    int unk1 = ifs.readu8();
    std::string name;
    ifs.readFixedLenString(name, 7);
    int size = ifs.readu32le();
    unsigned int nextP = ifs.readu32le();
    
    cout << "Link " << num << ": " << std::hex << pos << endl;
    cout << "  Unk1: " << std::hex << unk1 << endl;
    cout << "  Name: " << name << endl;
    cout << "  Size: " << std::hex << size << endl;
    
    if (nextP == 0) break;
    
    unsigned int next = nextP - pointerBase;
    if ((next < heapStart) || (next >= heapEnd)) {
      cout << "Broken heap at " << pos << "?" << endl;
      break;
    }
    
    ifs.seek(next);
    ++num;
  }*/
  
  TBufStream ifs;
  ifs.open("disc/files/DATA/YUNA3.FON");
  while (!ifs.eof()) {
    int next = ifs.readu16be();
    ifs.seekoff(-2);
    ifs.writeu16le(next);
  }
  
  ifs.save("test.bin");
  
  return 0;
}
