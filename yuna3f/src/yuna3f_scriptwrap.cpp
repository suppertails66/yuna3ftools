//#include "psx/PsxPalette.h"
#include "yuna3/Yuna3ScriptReader.h"
#include "yuna3/Yuna3LineWrapper.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

//const static int textCharsStart = 0x10;

using namespace std;
using namespace BlackT;
using namespace Psx;

TThingyTable table;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yuna 3 FE script wrapper" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>"
      << " [tablefile] [widthfile] [glyphwidthfile] [kerningmatrixfile] [textcharsstart]"
      << " [nonstd_flag]" << endl;
    return 0;
  }
  
  string infile = string(argv[1]);
  string outfile = string(argv[2]);
  
  string tableName = "table/yuna3_en.tbl";
  if (argc >= 4) tableName = string(argv[3]);
  
  string fontWidthFileName = "out/font/default/width.bin";
  if (argc >= 5) fontWidthFileName = string(argv[4]);
  
  string glyphWidthFileName = "out/font/default/glyph_width.bin";
  if (argc >= 6) glyphWidthFileName = string(argv[5]);
  
  string kerningFileName = "out/font/default/kerning_matrix.bin";
  if (argc >= 7) kerningFileName = string(argv[6]);
  
  int textCharsStart = 0x10;
  if (argc >= 8)
    textCharsStart = TStringConversion::stringToInt(string(argv[7]));
  
  bool isNonstd = false;
  if (argc >= 9)
    isNonstd = (TStringConversion::stringToInt(string(argv[8])) != 0);
  
  TThingyTable table;
//  table.readSjis(tableName.c_str());
  table.readUtf8(tableName.c_str());
  
  // wrap script
  {
    // read size table
    Yuna3LineWrapper::CharSizeTable sizeTable;
    {
      TBufStream ifs;
      ifs.open(fontWidthFileName.c_str());
      TBufStream glyphIfs;
      glyphIfs.open(glyphWidthFileName.c_str());
      int pos = 0;
      while (!ifs.eof()) {
        Yuna3LineWrapper::CharSizeTableEntry entry;
        entry.advanceWidth = ifs.readu8();
        entry.glyphWidth = glyphIfs.readu8();
        sizeTable[textCharsStart + (pos++)] = entry;
      }
    }
    
    // HACK: read hardcoded emph mode table
    // TODO
    Yuna3LineWrapper::CharSizeTable sizeTableEmph;
    {
      TBufStream ifs;
      ifs.open("out/font/default/width.bin");
      TBufStream glyphIfs;
      glyphIfs.open("out/font/default/glyph_width.bin");
      int pos = 0;
      while (!ifs.eof()) {
        Yuna3LineWrapper::CharSizeTableEntry entry;
        entry.advanceWidth = ifs.readu8();
        entry.glyphWidth = glyphIfs.readu8();
        sizeTableEmph[textCharsStart + (pos++)] = entry;
      }
    }
    
    Yuna3LineWrapper::KerningMatrix kerningMatrix(
      sizeTable.size(), sizeTable.size());
    {
      TBufStream ifs;
      ifs.open(kerningFileName.c_str());
      for (int j = 0; j < kerningMatrix.h(); j++) {
        for (int i = 0; i < kerningMatrix.w(); i++) {
          kerningMatrix.data(i, j) = ifs.get();
        }
      }
    }
    
    // HACK: read hardcoded emph kerning matrix
    // TODO
    Yuna3LineWrapper::KerningMatrix kerningMatrixEmph(
      sizeTableEmph.size(), sizeTableEmph.size());
    {
      TBufStream ifs;
      ifs.open("out/font/default/kerning_matrix.bin");
      for (int j = 0; j < kerningMatrixEmph.h(); j++) {
        for (int i = 0; i < kerningMatrixEmph.h(); i++) {
          kerningMatrixEmph.data(i, j) = ifs.get();
        }
      }
    }
    
    {
      TBufStream ifs;
      ifs.open((infile).c_str());
      
      TLineWrapper::ResultCollection results;
      Yuna3LineWrapper(ifs, results, table, sizeTable, sizeTableEmph,
                       kerningMatrix, kerningMatrixEmph,
                       isNonstd)();
      
      if (results.size() > 0) {
        TOfstream ofs(outfile.c_str());
        for (int i = 0; i < results.size(); i++) {
          ofs.write(results[i].str.c_str(), results[i].str.size());
        }
      }
    }
  }
  
  return 0;
}

