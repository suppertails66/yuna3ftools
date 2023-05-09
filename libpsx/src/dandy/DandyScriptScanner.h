#ifndef DANDYSCRIPTSCANNER_H
#define DANDYSCRIPTSCANNER_H


#include "util/TStream.h"
#include "util/TGraphic.h"
#include "util/TThingyTable.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace Psx {


class DandyScriptString {
public:
  int offset;
//  int size;
  std::string content;
  int triggerOpOffset;
  int portraitCode;
  
  DandyScriptString();
  void save(BlackT::TStream& ofs) const;
  void load(BlackT::TStream& ifs);
protected:
  
};

class DandyScriptScanResult {
public:
  typedef std::vector<DandyScriptString> ScriptStringCollection;
  typedef std::map<int, int> LocalToGlobalFontMap;
  
  // strings from script
  ScriptStringCollection strings;
  
  // mapping of local font codepoints to master font
  LocalToGlobalFontMap fontMap;
  
  void save(BlackT::TStream& ofs) const;
  void load(BlackT::TStream& ifs);
  void exportSimpleScript(std::ostream& ofs,
                          BlackT::TThingyTable& table) const;
  void exportHtmlScript(std::ostream& ofs,
                          BlackT::TThingyTable& table,
                          std::string resultNum) const;
protected:
  std::string convertStringFromTable(
      std::string content) const;
  
};

class DandyFont {
public:
  typedef std::vector<BlackT::TGraphic> FontCharCollection;
  
  // if the given graphic matches an existing character in the font,
  // return the corresponding index.
  // otherwise, add the graphic to the font and return the new index.
  int addOrGetChar(const BlackT::TGraphic& grp);
  
  void exportFont(std::string outname) const;
  
  FontCharCollection fontChars;
protected:
  const static int masterFontCharsPerRow = 16;
  const static int charW = 16;
  const static int charH = 16;
  
};

class DandyScriptScanner {
public:
  typedef std::map<std::string, DandyScriptScanResult> NameToResultMap;
  
  DandyScriptScanner();
  
  void scanScript(BlackT::TStream& ifs, std::string name);
  
  void exportMasterFont(std::string outname) const;
  
  void saveScript(BlackT::TStream& ofs) const;
  void loadScript(BlackT::TStream& ifs);
  void exportSimpleScript(std::ostream& ofs,
                          BlackT::TThingyTable& table) const;
  void exportHtmlScript(std::ostream& ofs,
                          BlackT::TThingyTable& table) const;
  
  NameToResultMap results;
  DandyFont masterFont;
  
protected:
  const static int charW = 16;
  const static int charH = 16;
  const static int baseFontCodepoint = 0x8001;
  static BlackT::TGraphic nullCharGrp;
  
  typedef std::map<int, bool> FontCharUseMap;
  
  bool readScriptString(BlackT::TStream& ifs,
                        DandyScriptString& dst,
                        int fontCharLimit,
                        FontCharUseMap& fontUseMap);
  
};


}


#endif
