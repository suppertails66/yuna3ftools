#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TOpt.h"
#include "util/TSoundFile.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include "psx/PsxXa.h"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace BlackT;
//using namespace Discaster;
using namespace Psx;

const static int rawSectorSize = 0x930;

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

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "PlayStation XA decoder" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>"
      << " [options]" << endl;
    cout << "Note that input should be in raw-disc (0x930 bytes per sector) format."
      << endl;
    cout << "Options:" << endl;
    cout << "  -i   Ignore first N sectors of input" << endl;
    
    return 0;
  }
  
  string inFile(argv[1]);
  string outFile(argv[2]);
  
  int initialOffset = 0;
  TOpt::readNumericOpt(argc, argv, "-i", &initialOffset);
  initialOffset *= rawSectorSize;
  
  TSoundFile dst;
  
  {
    TBufStream ifs;
    ifs.open(inFile.c_str());
    ifs.seekoff(initialOffset);
    PsxXa::decodeXa(ifs, dst);
  }
  
  {
    TBufStream ofs;
    dst.exportWav(ofs);
    ofs.save(outFile.c_str());
  }
  
  return 0;
}
