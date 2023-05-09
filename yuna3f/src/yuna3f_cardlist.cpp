#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TThingyTable.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxXa.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace BlackT;
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



int loadAddr = 0x80108000;
int unitStructSize = 0x60;

TThingyTable table;

std::string getString(TStream& ifs) {
  std::string str;
  while (ifs.peek() != 0) {
    TThingyTable::MatchResult result = table.matchId(ifs);
    str += table.getEntry(result.id);
  }
  return str;
}

int main(int argc, char* argv[]) {
  if (argc < 1) {
    std::cout << "Yuna 3 card list display program" << std::endl;
    std::cout << "Usage: " << argv[0] << std::endl;
    return 0;
  }
  
  table.readUtf8("table/sjis_utf8_yuna3.tbl");
  
  TBufStream ifs;
  ifs.open("disc/files/BIN/ALBUM.BIN");
  for (int i = 0; i < 55; i++) {
    int basePos = 0x4068 + (i * (8 * 4));
    
    std::cout << "== Card " << i + 1 << " ==" << std::endl;
    for (int j = 0; j < 8; j++) {
      ifs.seek(basePos + (j * 4));
      int strPtr = ifs.readu32le();
      ifs.seek(strPtr - loadAddr);
      std::string str = getString(ifs);
      std::cout << str << std::endl;
    }
  }
  
  return 0;
}
