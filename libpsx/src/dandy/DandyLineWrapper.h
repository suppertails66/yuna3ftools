#ifndef DANDYLINEWRAPPER_H
#define DANDYLINEWRAPPER_H


#include "util/TStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "util/TLineWrapper.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>

namespace Psx {


class DandyLineWrapper : public BlackT::TLineWrapper {
public:
  typedef std::map<int, int> CharSizeTable;

  DandyLineWrapper(BlackT::TStream& src__,
//                  BlackT::TStream& dst__,
                  ResultCollection& dst__,
                  const BlackT::TThingyTable& thingy__,
                  CharSizeTable sizeTable__,
                  int xSize__ = -1,
                  int ySize__ = -1);
  
  /**
   * Return width of a given symbol ID in "units" --
   * pixels, characters, whatever is compatible with the specified xSize.
   */
  virtual int widthOfKey(int key);
  
  virtual int advanceWidthOfKey(int key);
  
  /**
   * Return true if a given symbol ID is considered a word boundary.
   * For English text, this will usually be whitespace characters.
   * Linebreaks can and should be included in this category.
   */
  virtual bool isWordDivider(int key);
  
  /**
   * Return true if a given symbol ID constitutes a linebreak.
   * A linebreak is, by default, considered to do the following:
   *   - increment the yPos
   *   - reset the xPos to zero
   */
  virtual bool isLinebreak(int key);
  
  /**
   * Return true if a given symbol ID constitutes a box clear.
   * A box clear is, by default, considered to do the following:
   *   - reset the xPos to zero
   *   - reset the yPos to zero
   */
  virtual bool isBoxClear(int key);
  
  /**
   * This function is called immediately before the next word would normally
   * be output when the following conditions are met:
   *   a.) yPos == ySize, and
   *   b.) the current word's computed width will, when added to xPos, exceed
   *       xSize (necessitating a linebreak)
   * The implementation should handle this as appropriate for the target,
   * such as by outputting a wait/clear command, emitting an error, etc.
   */
  virtual void onBoxFull();
  
  /**
   * Returns the string used for linebreaks symbol.
   */
  virtual std::string linebreakString() const;
  
//  virtual int linebreakHeight() const;
  virtual void handleManualLinebreak(TLineWrapper::Symbol result, int key);
  
  virtual void onSymbolAdded(BlackT::TStream& ifs, int key);
  virtual void afterSymbolAdded(BlackT::TStream& ifs, int key);
  virtual void beforeLinebreak(LinebreakSource clearSrc,
                             int key);
  virtual void afterLinebreak(LinebreakSource clearSrc,
                             int key);
  virtual void beforeBoxClear(BoxClearSource clearSrc,
                             int key);
  virtual void afterBoxClear(BoxClearSource clearSrc,
                             int key);
protected:

//  enum ClearMode {
//    clearMode_default,
//    clearMode_messageSplit
//  };

  enum BreakMode {
    breakMode_single,
    breakMode_double
  };
  
  enum AlignMode {
    align_none,
    align_left,
    align_right,
    align_center
  };
  
  enum PadMode {
    pad_off,
    pad_on
  };
  
  enum AutoPadMode {
    autopad_none,
    autopad_menu
  };

//  bool waitPending;
  CharSizeTable sizeTable;
  int xBeforeWait;
//  ClearMode clearMode;
  BreakMode breakMode;
  
  bool errorOnOverflow;
  
  AlignMode alignMode;
//  int alignWidth;
//  PadMode padMode;
  int padWidth;
  AutoPadMode autoPadMode;
  
  virtual bool processUserDirective(BlackT::TStream& ifs);
  
  void findContentStart(BlackT::TStream& ifs);
  void alignCurrentScript();
  void padCurrentScript();
  std::string generatePaddingString(int width);
};


}


#endif
