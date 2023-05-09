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

int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "PlayStation EXE expander" << endl;
    cout << "Usage: " << argv[0] << " <infile> <presize> <postsize> <outfile>"
      << endl;
    cout << "NOTE: Pre-size does not include EXE header." << endl;
    
    return 0;
  }
  
  string inFile(argv[1]);
  int preSize(TStringConversion::stringToInt(string(argv[2])));
  int postSize(TStringConversion::stringToInt(string(argv[3])));
  string outFile(argv[4]);
  
  TBufStream ifs;
  ifs.open(inFile.c_str());
  
  TBufStream ofs;
  ofs.writeFrom(ifs, 0x800);
  
  {
//    int padSize = preSize - ofs.remaining();
    for (int i = 0; i < preSize; i++) ofs.put(0x00);
  }
  
  ofs.writeFrom(ifs, ifs.remaining());
  
  for (int i = 0; i < postSize; i++) ofs.put(0x00);
  
//  {
//    int padSize = postSize - (ifs.size() - 0x800);
//    for (int i = 0; i < padSize; i++) ofs.put(0x00);
//  }
  
  // update size field
  ofs.seek(0x1C);
  ofs.writeu32le(ofs.size() - 0x800);
  
  ofs.save(outFile.c_str());
  
  return 0;
}
