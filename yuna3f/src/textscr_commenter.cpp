#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;

typedef std::map<unsigned long int, std::vector<int> > AddrToOpMap;
AddrToOpMap textscrAddrToOpMap;

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

const int loadAddr = 0x80110000;
const int tableBase = 0x9F018;
const int numOps = 0xE6;
std::string msg = "; script op ";

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Textscript disassembly comment label adder" << endl;
    cout << "Usage: " << argv[0] << " [dis_infile] [dis_outfile]" << endl;
    
    return 0;
  }
  
  std::ifstream fileifs(argv[1]);
  std::ofstream fileofs(argv[2]);
  
  {
    TBufStream ifs;
//    ifs.open("files_sat/1ST_MAHO.BIN");
    ifs.open("base/SLPS_014.51_noheader");
    ifs.seek(tableBase);
    for (int i = 0; i < numOps; i++) {
      unsigned int value = ifs.readu32le();
      cerr << hex << i << " " << value << endl;
      
      textscrAddrToOpMap[value].push_back(i);
    }
  }
  
  while (fileifs.good()) {
    fileifs.get();
    if (!fileifs.good()) break;
    fileifs.unget();
    
    std::string line;
    std::getline(fileifs, line);
    
    std::istringstream ifs;
    ifs.str(line);
    
    std::string start;
    ifs >> start;
    
    // comments
    if ((start.size() == 0)
        || ((start.size() > 0) && (start[0] == ';'))) {
      fileofs << line << endl;
      continue;
    }
//    TBufStream ifs;
//    ifs.writeString(line);
    
    std::string numstr = string("0x") + start;
    unsigned long int addr = TStringConversion::stringToInt(numstr);
    
//    cout << hex << addr << endl;
//    char c; cin >> c;
    
    AddrToOpMap::iterator findIt = textscrAddrToOpMap.find(addr);
    if (findIt != textscrAddrToOpMap.end()) {
      fileofs << endl;
      for (unsigned int i = 0; i < findIt->second.size(); i++) {
        int opNum = findIt->second[i];
        
        fileofs << msg << as2bHex(opNum) << endl;
        
      }
      
      fileofs << endl;
      fileofs << line << endl;
      
//      int opNum = findIt->second;
      
//      fileofs << endl << "; script op " << as2bHex(opNum) << endl << endl;
//      fileofs << line << endl;
    }
    else {
      fileofs << line << endl;
    }
  }
  
  return 0;
}
