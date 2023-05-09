#ifndef POMPLMDATA_H
#define POMPLMDATA_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>

namespace Psx {


class PomPlmStringScanResult {
public:
  int offset;
  int size;
  std::string content;
  
  // all existing strings appear to be referenced exactly once,
  // so no need for a list
  int scriptRefOffset;
  int scriptRefCode;
  
};

class PomPlmStringScanResults {
public:
  std::vector<PomPlmStringScanResult> results;
};

class PomPlmScriptString {
public:
  std::string content;
  int origOffset;
  int origSize;
  int scriptRefStartCode;
  int scriptRefStart;
  int scriptRefEnd;
  
  std::string translationPlaceholder;
};

class PomPlmScriptStringSet {
public:
  std::vector<PomPlmScriptString> strings;
};

class PomPlmData {
public:
//  typedef BlackT::TArray<BlackT::TByte> DataArray;
//  typedef std::vector< BlackT::TArray<BlackT::TByte> > DataArrayCollection;

//  PomPlmData();
  
//  void read(BlackT::TStream& ifs);

  static void stringScan(
    BlackT::TStream& ifs, BlackT::TThingyTable& table,
    PomPlmStringScanResults& dst);

  static void formOutputStrings(
    BlackT::TStream& ifs, const PomPlmStringScanResults& src,
    PomPlmScriptStringSet& dst);
  
  const static int printSequenceFullSize = 11;
  const static int jumpSequenceFullSize = 3;
  const static std::string printSequenceCheckStr;
  const static std::string luluNameSeq;
  
  const static int op_pushStrPtr = 0x33;
  const static int op_specialPushStrPtr = 0x3E;
  const static int op_pushCmdCode = 0x44;
  const static int op_jump = 0x77;
  const static int op_conditionalJump = 0x72;
protected:
  static PomPlmStringScanResult attemptStringRead(
    BlackT::TStream& ifs, BlackT::TThingyTable& table);
  
  static bool checkSequence(BlackT::TStream& ifs, std::string checkStr);
  static bool checkPrintSequence(BlackT::TStream& ifs);
  
  static bool endsInParagraphBreak(std::string str);
  static std::string getNametagSequence(std::string str);
  static bool jumpToOffsetExists(BlackT::TStream& ifs, int offset, int limit);
  
};


}


#endif
