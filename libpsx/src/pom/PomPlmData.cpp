#include "pom/PomPlmData.h"
#include "pom/PomCmp.h"
#include "pom/PomPak.h"
#include "pom/PomPlmStringScanException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include "util/TByte.h"
#include <vector>
#include <map>
#include <iostream>

using namespace BlackT;

namespace Psx {


//     33 [stringOffset]  ; pushString(offset)
//     48 0100  ; textSubOp(0x100)
//     79 0014  ; call(0x14)
//     7E 04    ; moveStack(4)
const std::string PomPlmData::printSequenceCheckStr(
  "\x48\x00\x01\x79\x14\x00\x7E\x04", 8);
const std::string PomPlmData::luluNameSeq(
  "\x48\x0C\x01\x79\x14\x00\x7E\x02", 8);


void PomPlmData::stringScan(
    BlackT::TStream& ifs, BlackT::TThingyTable& table,
    PomPlmStringScanResults& dst) {
//  ifs.seek(6);
//  int offset1 = ifs.readu16le();
//  int offset2 = ifs.readu16le();
//  if (offset1 != offset2) {
//    std::cout << "mismatch: " << std::hex << offset1 << " " << offset2
//      << std::endl;
//  }
  
  ifs.seek(0x10);
  
  if (ifs.readu8() != 0x79) {
    throw PomPlmStringScanException(T_SRCANDLINE,
                                    "PomPlmData::stringScan()",
                                    "bad initial jump");
  }
  
  int runnerOffset = ifs.readu16le();
  ifs.seek(runnerOffset + 4);
  
  if (ifs.readu8() != 0x78) {
    throw PomPlmStringScanException(T_SRCANDLINE,
                                    "PomPlmData::stringScan()",
                                    "bad switch handler jump");
  }
  
  int switchOffset = ifs.readu16le();
  ifs.seek(switchOffset);
  
  int switchSize = ifs.readu16le();
  ifs.seekoff(switchSize * 4);
  
//  for (int i = 0; i < 10; i++) {
//    std::cerr << std::hex << ifs.readu8() << " ";
//  }
//  std::cerr << std::endl;
  
  ifs.seekoff(2);
  // map 00 is slightly different from everything else here,
  // so check for that
  if (ifs.readu16be() == 0x0104) {
    // normal map
    ifs.seekoff(6);
  }
  else {
    // map 00
//    ifs.seekoff(7);
    // wait, no, it's actually the same as normal...?
    ifs.seekoff(6);
  }
  
  int stringBlockStart = ifs.tell();
//  std::cout << "blockstart: " << std::hex << stringBlockStart << std::endl;
  
  // scan for strings
  while (!ifs.eof()) {
    int basePos = ifs.tell();
    try {
      PomPlmStringScanResult result = attemptStringRead(ifs, table);
      
//      if (basePos == 0x2FE9) {
//        std::cerr << ifs.size() << " " << result.content << std::endl;
//      }
      
      int endPos = ifs.tell();
      
      ifs.seek(1);
//      bool hasStdRef = false;
//      bool hasNonstdRef = false;
      int stdRefCount = 0;
      int nonstdRefCount = 0;
      while (ifs.tell() < stringBlockStart - 1) {
        int pos = ifs.tell();
        if (ifs.readu16le() == result.offset) {
          ifs.seek(pos - 1);
          int code = ifs.readu8();
          // 0x33 = standard "push string to stack" op,
          //        used for almost everything
          // 0x3E = some weird alternate thing,
          //        used only in map 00 (pom community) for special strings
          if ((code == 0x33) || (code == 0x3E)) {
//            std::cout << "  stdref: " << std::hex << code << " " << pos << std::endl;
            result.scriptRefOffset = pos - 1;
            result.scriptRefCode = code;
            ++stdRefCount;
          }
          else {
//            std::cout << "  nonstdref: " << std::hex << code << " " << pos << std::endl;
            ++nonstdRefCount;
          }
          
          ifs.seek(pos + 1);
        }
        else {
          ifs.seek(pos + 1);
        }
      }
      
/*      if (stdRefCount == 1) {
        
      }
      else if (stdRefCount > 1) {
        std::cout << "  multiref" << std::endl;
      }
      else {
        std::cout << "  no refs??" << std::endl;
      }
//      if ((nonstdRefCount != 0) && (stdRefCount == 0)) {
//        std::cout << "both" << std::endl;
//      } */
      
      ifs.seek(endPos);
      
      // ignore unreferenced strings
      // (these are always null... except i guess the subop 0x109
      // commands in map 0, which will need to be specially handled)
      if (stdRefCount == 0) continue;
      
      dst.results.push_back(result);
//      std::cout << std::hex << basePos << ": " << result.content << std::endl;
    }
    catch (PomPlmStringScanException& e) {  
      // retry from next pos
      ifs.seek(basePos + 1);
    }
  }
}

PomPlmStringScanResult PomPlmData::attemptStringRead(
    BlackT::TStream& ifs, BlackT::TThingyTable& table) {
  PomPlmStringScanResult result;
  result.offset = ifs.tell();
  
  while (!ifs.eof()) {
    // check for terminator
    if (ifs.peek() == 0x00) {
      ifs.get();
      result.size = ifs.tell() - result.offset;
      return result;
    }
    
    // values use a variable-size encoding
    int nextValue = 0;
    // this may be the dumbest loop i've ever written
    int nextSize = 0;
    do {
      int next = ifs.readu8();
      nextValue |= ((next & 0x7F) << (7 * nextSize));
      ++nextSize;
      if (next & 0x80)
        continue;
      else
        break;
    } while (!ifs.eof());
    if (ifs.eof()) break;
    
    if (nextSize > 2) {
      throw PomPlmStringScanException(T_SRCANDLINE,
                                      "PomPlmData::attemptStringRead()",
                                      "bad size");
    }
    
    if (!table.hasEntry(nextValue)) {
      throw PomPlmStringScanException(T_SRCANDLINE,
                                      "PomPlmData::attemptStringRead()",
                                      "no matching table entry");
    }
    
    std::string tableEntry = table.getEntry(nextValue);
    result.content += tableEntry;
  }
  
  // we can only get here if the string is not terminated
  throw PomPlmStringScanException(T_SRCANDLINE,
                                  "PomPlmData::attemptStringRead()",
                                  "unterminated string");
}

void PomPlmData::formOutputStrings(
    BlackT::TStream& ifs, const PomPlmStringScanResults& src,
    PomPlmScriptStringSet& dst) {
  std::vector<PomPlmStringScanResult>::const_iterator it
    = src.results.cbegin();
  
  int stringsStart = 0;
  if (!src.results.empty()) stringsStart = src.results[0].offset;
  
  while (it != src.results.cend()) {
    const PomPlmStringScanResult& baseItem = *it;
    std::vector<PomPlmStringScanResult>::const_iterator baseItemIt
      = it;
    
    PomPlmScriptString output;
    output.content = baseItem.content;
    output.origOffset = baseItem.offset;
    output.origSize = baseItem.size;
    output.scriptRefStartCode = baseItem.scriptRefCode;
    output.scriptRefStart = baseItem.scriptRefOffset;
    // if left at placeholder -1 value, indicates in-place assignment
    output.scriptRefEnd = -1;
    
    // verify that the target position contains a standard print
    // sequence
    ifs.seek(baseItem.scriptRefOffset);
    if (!checkPrintSequence(ifs)) {
      // check for nametag
      std::string nametag = getNametagSequence(output.content);
      output.translationPlaceholder += nametag;
      
      if (endsInParagraphBreak(output.content)) {
        output.translationPlaceholder += "\n\\p\n\n";
      }
      
      dst.strings.push_back(output);
      ++it;
      
      // initial paragraph check for translation placeholder
/*      if (output.content.size() >= 2) {
        if (output.content.substr(
              output.content.size() - 2, std::string::npos)
            .compare("\\p") == 0) {
          output.translationPlaceholder += "\n\\p\n\n";
        }
      } */
      
      continue;
    }
    
    // the goal here is to determine how many messages the base item
    // can be concatenated with
    
    output.scriptRefEnd = output.scriptRefStart + printSequenceFullSize;
    
    bool currentParagraphNametagFound = false;
    int currentParagraphStartPos = 0;
    while (true) {
      // if current message ends in "\p" pause sequence, we're done
/*      if (output.content.size() >= 2) {
        if (output.content.substr(
              output.content.size() - 2, std::string::npos)
            .compare("\\p") == 0) {
          // TODO: do we want this broken up per text box
          // or per dialogue sequence?
//          break;
          output.content += "\n\n";
          
          output.translationPlaceholder += "\n\\p\n\n";
        }
      } */
      
      if (endsInParagraphBreak(output.content)) {
        output.content += "\n\n";
        output.translationPlaceholder += "\n\\p\n\n";
        
//        currentParagraphNametagFound = false;
//        currentParagraphStartPos = output.content.size();
      }
      
      // check for lulu name sequence(s)
      while (true) {
        ifs.seek(output.scriptRefEnd);
        if (ifs.remaining() < luluNameSeq.size()) break;
        
        if (checkSequence(ifs, luluNameSeq)) {
          output.scriptRefEnd += luluNameSeq.size();
          // FIXME?
          output.content += "[lulu]";
        }
        else {
          break;
        }
      }
      
      // check for concatenateable sequences
      
      ++it;
      if (it == src.results.cend()) {
        // can't concatenate past end of input
        --it;
        break;
      }
//      std::vector<PomPlmStringScanResult>::const_iterator nextItemIt
//        = it;
      const PomPlmStringScanResult& nextItem = *it;
      
      // next string is not null?
//      if (nextItem.content.size() == 0) {
//        --it;
//        break;
//      }
      
      // next string starts at end of current one?
      if (nextItem.scriptRefOffset != output.scriptRefEnd) {
        --it;
        break;
      }
      
      // next string is a standard sequence?
      ifs.seek(nextItem.scriptRefOffset);
      if (!checkPrintSequence(ifs)) {
        --it;
        break;
      }
      
      // next string is not the target of a jump?
      // (GODDAMN OPTIMIZERS)
      if (jumpToOffsetExists(ifs, nextItem.scriptRefOffset, stringsStart)) {
        // check for heuristic misdetection in some maps
        if (output.origOffset != 0x1103) {
//          std::cerr << "optimized jump: "
//            << std::hex << output.origOffset << std::endl;
          --it;
          break;
        }
      }
      
      // if existing message ends in "\n" linebreak sequence,
      // add a linebreak for readability
      if (output.content.substr(
            output.content.size() - 2, std::string::npos)
          .compare("\\n") == 0) {
        output.content += "\n";
      }
      
      // concatenate
      output.content += nextItem.content;
      output.scriptRefEnd += printSequenceFullSize;
      
      // check for nametag
//      if (lastWasParagraphBreak) {
//        std::string nametag = getNametagSequence(nextItem.content);
//        output.translationPlaceholder += nametag;
//      }
//      if (!currentParagraphNametagFound) {
        if (endsInParagraphBreak(nextItem.content)) {
          std::string nametag = getNametagSequence(output.content
            .substr(currentParagraphStartPos, std::string::npos));
          if (!nametag.empty()) {
            output.translationPlaceholder += nametag;
//            currentParagraphNametagFound = true;
          }
          
          currentParagraphStartPos = output.content.size();
        }
//      }
    }
    
    // remove trailing linebreaks
    while ((output.content.size() > 0)
           && (output.content[output.content.size() - 1] == '\n')) {
      output.content = output.content.substr(0, output.content.size() - 1);
    }
    while ((output.translationPlaceholder.size() > 0)
           && (output.translationPlaceholder[
                output.translationPlaceholder.size() - 1] == '\n')) {
      output.translationPlaceholder
        = output.translationPlaceholder.substr(
          0, output.translationPlaceholder.size() - 1);
    }
    
    // first, as a special condition, check if message
    // is followed by command 48 010C (print lulu's name).
    // if so 
    // we NEVER concatenate if:
    // - the base message is not the standard output sequence:
    //     33 [stringOffset]  ; pushString(offset)
    //     48 0100  ; textSubOp(0x100)
    //     79 0014  ; call(0x14)
    //     7E 04    ; moveStack(4)
    // - the base message ends in a \p (wait for input) command,
    //   signalling the end of a text box
    // assuming those conditions are met, we concatenate while:
    // - all the following prerequisites are met:
    //  - the next item's scriptRefOffset immediately follows the current
    //    one's
    // - one of the following prerequisites is met:
    //   - the next item also uses the standard output sequence
    
    // check if we need to *prepend* a special sequence to the start of the
    // output
    while (true) {
      if (output.scriptRefStart < luluNameSeq.size()) break;
      ifs.seek(output.scriptRefStart - luluNameSeq.size());
      if (checkSequence(ifs, luluNameSeq)) {
        output.scriptRefStart -= luluNameSeq.size();
        // FIXME?
        output.content = (std::string("[lulu]") + output.content);
      }
      else {
        break;
      }
    }
    
    dst.strings.push_back(output);
    
    ++it;
  }
}

bool PomPlmData::checkSequence(BlackT::TStream& ifs, std::string checkStr) {
  if (ifs.remaining() < checkStr.size()) return false;
  
  for (unsigned int i = 0; i < checkStr.size(); i++) {
    if (ifs.get() != checkStr[i]) return false;
  }
  
  return true;
}

bool PomPlmData::checkPrintSequence(BlackT::TStream& ifs) {
  if (ifs.remaining() < (printSequenceCheckStr.size() + 3)) return false;
  if (ifs.readu8() != 0x33) return false;
  // skip target string offset
  ifs.seekoff(2);
  
  return checkSequence(ifs, printSequenceCheckStr);
}

bool PomPlmData::endsInParagraphBreak(std::string str) {
  if (str.size() >= 2) {
    if (str.substr(str.size() - 2, std::string::npos).compare("\\p") == 0) {
      return true;
    }
  }
  
  return false;
}

std::string PomPlmData::getNametagSequence(std::string str) {
  if (str.size() < 4) return "";
  
  TBufStream ifs;
  ifs.writeString(str);
  ifs.seek(0);
  
  // "〈"
  if (ifs.readu16be() != 0x8171) return "";
  
  while (true) {
    if (ifs.remaining() < 2) return "";
    int next = ifs.readu16be();
    // "〉"
    if (next == 0x8172) break;
    if ((next & 0x8000) == 0) ifs.seekoff(-1);
  }
  
  if (ifs.remaining() < 2) return "";
  if (ifs.get() != '\\') return "";
  if (ifs.get() != 'n') return "";
  
  int endPos = ifs.tell();
  ifs.seek(0);
  
  std::string result;
  for (int i = 0; i < endPos; i++) result += ifs.get();
  result += "\n";
  return result;
}

bool PomPlmData::jumpToOffsetExists(BlackT::TStream& ifs, int offset,
                                    int limit) {
  int startPos = ifs.tell();
  bool result = false;
  
  for (int i = 0; i < (limit - jumpSequenceFullSize + 1); i++) {
    ifs.seek(i);
    
    int next = ifs.readu8();
    if ((next != op_jump)
        && (next != op_conditionalJump)) continue;
    
    if (ifs.readu16le() == offset) {
      result = true;
      break;
    }
  }
  
  ifs.seek(startPos);
  return result;
}


}
