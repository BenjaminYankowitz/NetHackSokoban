#ifndef _SOKOBANMAP__H__
#define _SOKOBANMAP__H__
#include "HashMap.h"
#include "typedef.h"
#include "Pos.h"
#include <string>

class SokobanMap {
public:
    SokobanMap(const std::string &strMap);
    HashMap<Pos, myChar> cordToId;
    std::vector<Pos> idToCord;
    Pos holePos;
    std::vector<bool> initStateBolder;
    std::vector<uint16_t> distToExit;
    myChar holeNumber;
    std::vector<myChar> outSideHoleId;
    SokobanMap() {}
};

#endif