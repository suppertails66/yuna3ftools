#ifndef DANDYIMGEXTR_H
#define DANDYIMGEXTR_H


#include "util/TStream.h"
#include "util/TGraphic.h"
#include <string>

namespace Psx {


class DandyImgExtr {
public:
  
//  static void extract(BlackT::TStream& ifs, std::string outname);
//  static void getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp);
  
  static void extract(BlackT::TStream& ifs, std::string outname);
  static void getGraphic(BlackT::TStream& ifs, BlackT::TGraphic& grp);
  
protected:
  const static double colorScale;
  
};


}


#endif
