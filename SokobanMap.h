// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

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