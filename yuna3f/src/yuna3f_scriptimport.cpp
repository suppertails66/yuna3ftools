#include "yuna3/Yuna3TranslationSheet.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TFileManip.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TCsv.h"
#include "util/TParse.h"
#include "util/TThingyTable.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;
using namespace BlackT;
using namespace Psx;

Yuna3TranslationSheet scriptSheet;
//TThingyTable tableStd;

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}

void importScript(string basename, bool substitutionsOn = false,
                  bool forcePrefixSuffixLinebreak = false) {
  TBufStream ifs;
  ifs.open((string("script/") + basename).c_str());
  
  TBufStream ofs;
  while (!ifs.eof()) {
    std::string line;
    ifs.getLine(line);
    
    TBufStream lineIfs;
    lineIfs.writeString(line);
    lineIfs.seek(0);
    
    bool success = false;
    
    TParse::skipSpace(lineIfs);
    if (TParse::checkChar(lineIfs, '#')) {
      TParse::matchChar(lineIfs, '#');
      
      std::string name = TParse::matchName(lineIfs);
      TParse::matchChar(lineIfs, '(');
      
      for (unsigned int i = 0; i < name.size(); i++) {
        name[i] = toupper(name[i]);
      }
      
      bool isDirectImport = (name.compare("IMPORT") == 0);
      if (isDirectImport
          || (name.compare("IMPORTIFEXISTS") == 0)) {
        std::string id = TParse::matchString(lineIfs);
//        cerr << id << endl;
        
        if (!isDirectImport && !scriptSheet.hasStringEntryForId(id)) {
          // return as a success if target file does not exist
          // so the import directive will be removed
          success = true;
        }
        else {
          Yuna3TranslationSheetEntry str = scriptSheet.getStringEntryById(id);
          
  //        if (substitutionsOn) {
  //          str.stringContent = doSubstitutions(str.stringContent);
  //        }
          
          // HACK: ignore "empty" cells for now
          if (!(str.stringContent.empty()
                && str.stringPrefix.empty()
                && str.stringSuffix.empty())) {
            ofs.writeString(str.stringPrefix);
//            ofs.put('\n');
            
            // HACK: insert linebreaks between prefix/suffix and main content
            // only if next line immediately begins with a directive.
            // this would not matter except that, because the line wrapper
            // works on one line at a time, and needs to be able to provide
            // the next literal so it can decide whether sequences like "...}"
            // can be line-wrapped, the opening and closing quotes that are
            // pre/postpended to the text need to be on the same line as the
            // main text.
            if (forcePrefixSuffixLinebreak
                || ((str.stringContent.size() > 0)
                  && (str.stringContent[0] == '#'))) {
              ofs.put('\n');
            }
            
            ofs.writeString(str.stringContent);
            
            // HACK
            if (forcePrefixSuffixLinebreak
                || ((str.stringSuffix.size() > 0)
                    && (str.stringSuffix[0] == '#'))) {
              ofs.put('\n');
            }
            
            ofs.writeString(str.stringSuffix);
            ofs.put('\n');
          }
          
          TParse::matchChar(lineIfs, ')');
          success = true;
        }
      }
    }
    
    if (!success) {
      ofs.writeString(line);
      ofs.put('\n');
    }
  }
  
  std::string outname = (string("out/scripttxt/") + basename).c_str();
  TFileManip::createDirectoryForFile(outname.c_str());
  ofs.save(outname.c_str());
}

int main(int argc, char* argv[]) {
  if (argc < 1) {
    cout << "Yuna 3 FE script importer" << endl;
    cout << "Usage: " << argv[0]
      << endl;
    return 0;
  }
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_main.csv");
  importScript("spec_main.txt");
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_mapscript.csv");
  importScript("spec_mapscript.txt");
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_new.csv");
  importScript("spec_new.txt");
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_system.csv");
  importScript("spec_system.txt");
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_video.csv");
  importScript("spec_video.txt");
  
  scriptSheet = Yuna3TranslationSheet();
  scriptSheet.importCsv("script/script_xa.csv");
  importScript("spec_xa.txt", false, true);
  
  return 0;
}
