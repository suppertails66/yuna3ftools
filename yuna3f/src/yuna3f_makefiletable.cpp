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
#include "util/TParse.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
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

struct FileTableEntry {
  int namePtr;
  std::string name;
  int sectorCount;
  int sectorNum;
  
  void write(TStream& ofs) const {
    ofs.writeu32le(namePtr);
    ofs.writeu32le(sectorCount);
    ofs.writeu32le(sectorNum);
  }
};

struct DiscReportEntry {
  std::string fullName;
  std::string name;
  int sectorNum;
  int byteSize;
  bool isRaw;
};

typedef std::map<int, std::string> PtrNameMap;
typedef std::map<std::string, int > NamePtrMap;
typedef std::map<int, DiscReportEntry> SectorDiscRepMap;

PtrNameMap ptrToName;
NamePtrMap nameToPtr;
SectorDiscRepMap sectorToDiscRep;

const static int exeBase = 0x80110000;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yuna 3 FE file table generator" << endl;
    // lol, "discaster report"
    cout << "Usage: " << argv[0] << " [discaster_report] [outfile]" << endl;
    return 0;
  }
  
  std::string discReportName(argv[1]);
  std::string outName(argv[2]);
  
  //==============
  // read in old file table to get pointer->name mapping
  //==============
  
  std::vector<FileTableEntry> fileTableEntries;
  
  {
    TBufStream ifs;
    ifs.open("base/SLPS_014.51_noheader");
    
    int tableBasePos = 0x801AF468 - exeBase;
    
    for (int i = 0; i < 349; i++) {
      ifs.seek(tableBasePos + (i * 12));
      
      FileTableEntry entry;
      entry.namePtr = ifs.readu32le();
      entry.sectorCount = ifs.readu32le();
      entry.sectorNum = ifs.readu32le();
      
      ifs.seek(entry.namePtr - exeBase);
      ifs.readCstrString(entry.name);
      
      fileTableEntries.push_back(entry);
      ptrToName[entry.namePtr] = entry.name;
      nameToPtr[entry.name] = entry.namePtr;
      
  //    std::cout << name << " " << sectorCount << " " << sectorNum << std::endl;
    }
  }
  
  //==============
  // read in disc report
  //==============
  
  {
    TBufStream ifs;
    ifs.open(discReportName.c_str());
    
    while (!ifs.eof()) {
      std::string line;
      ifs.getLine(line);
      
      if (line.size() <= 0) continue;
      
      TBufStream lineIfs;
      lineIfs.writeString(line);
      lineIfs.seek(0);
      
      DiscReportEntry entry;
      entry.fullName = TParse::getSpacedSymbol(lineIfs);
      entry.name = TParse::getSpacedSymbol(lineIfs);
      entry.sectorNum = TParse::matchInt(lineIfs);
      entry.byteSize = TParse::matchInt(lineIfs);
      std::string rawStr = TParse::getSpacedSymbol(lineIfs);
      if (rawStr.compare("raw") == 0)
        entry.isRaw = true;
      else
        entry.isRaw = false;
      
      // order files in table by position on disc, per original game
      // (though this probably isn't actually required)
      sectorToDiscRep[entry.sectorNum] = entry;
    }
  }
  
  //==============
  // produce output
  //==============
  
  std::vector<FileTableEntry> outputFileTableEntries;
  TBufStream ofs;
  
  for (SectorDiscRepMap::iterator it = sectorToDiscRep.begin();
       it != sectorToDiscRep.end();
       ++it) {
    DiscReportEntry repEntry = it->second;
    
    FileTableEntry tableEntry;
    // ignore entries not in original table
    NamePtrMap::iterator findIt = nameToPtr.find(repEntry.name);
    if (findIt == nameToPtr.end()) continue;
    
    tableEntry.namePtr = findIt->second;
    tableEntry.name = findIt->first;
    tableEntry.sectorNum = repEntry.sectorNum;
    // regular files = 0x800-byte sectors
    // raw files = 0x930-byte sectors, but are treated as though they had
    // 0x800-byte sectors for purposes of computing size
    // oh wait, discaster has already computed that in its report
//    if (repEntry.isRaw)
//      tableEntry.sectorCount = (((repEntry.byteSize / 0x930) * 0x800) + 0x7FF) / 0x800;
//    else
      tableEntry.sectorCount = (repEntry.byteSize + 0x7FF) / 0x800;
    
    // FIXME?
    // the sole discrepancy between the generated table and the original
    // when using the original files and layout is that the entry for
    // the main executable, SLPS_014.51, has its size listed as 0x180 sectors
    // in the original game, while the actual size of the file is 0x17C
    // sectors. i doubt the game even uses this entry, as the exe is loaded
    // by the bios and i'm pretty sure it's never unloaded, but we'll see.
    
    tableEntry.write(ofs);
    
//    cout << (ofs.size() / 12) << ": " << tableEntry.name
//      << " " << tableEntry.sectorCount
//      << " " << tableEntry.sectorNum << endl;
  }
  
  ofs.save(outName.c_str());
  
  return 0;
}
