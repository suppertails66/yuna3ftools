#ifndef DANDYSCRIPTREADER_H
#define DANDYSCRIPTREADER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Psx {


class DandyScriptReader {
public:
//  typedef std::map<int, int> CharSizeTable;

  struct ResultString {
    std::string str;
    int srcOffset;
    int srcSize;
    int srcOpOffset;
    int index;
    std::string name;
    int width;
    
    // for use during assembly
    bool autoPadded;
    
    ResultString()
      : autoPadded(false) { }
  };
  
  typedef std::vector<ResultString> ResultCollection;
  typedef std::map<int, ResultCollection> FileToResultMap;

  DandyScriptReader(BlackT::TStream& src__,
//                  BlackT::TStream& dst__,
//                  NesRom& dst__,
                  FileToResultMap& dst__,
                  const BlackT::TThingyTable& thingy__);
  
  bool operator()();
protected:

  BlackT::TStream& src;
  FileToResultMap& dst;
  BlackT::TThingyTable thingy;
  int lineNum;
  bool breakTriggered;
  int fileNum;
  
  BlackT::TBufStream currentScriptBuffer;
  int currentScriptSrcOffset;
  int currentScriptSrcSize;
  int currentScriptSrcOpOffset;
  int currentScriptIndex;
  std::string currentScriptName;
  int currentScriptWidth;
  
  void outputNextSymbol(BlackT::TStream& ifs);
  
//  bool checkSymbol(BlackT::TStream& ifs, std::string& symbol);
  
  void flushActiveScript();
  void resetScriptBuffer();
  
  void processDirective(BlackT::TStream& ifs);
  void processLoadTable(BlackT::TStream& ifs);
  void processStartMsg(BlackT::TStream& ifs);
  void processEndMsg(BlackT::TStream& ifs);
  void processIncBin(BlackT::TStream& ifs);
  void processBreak(BlackT::TStream& ifs);
  void processStartFile(BlackT::TStream& ifs);
  void processEndFile(BlackT::TStream& ifs);
  
  void loadThingy(const BlackT::TThingyTable& thingy__);
  
};


}


#endif
