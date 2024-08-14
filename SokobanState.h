// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#ifndef _SOKOBANSTATE__H__
#define _SOKOBANSTATE__H__
#include "HashMap.h"
#include "Pos.h"
#include "SokobanMap.h"
#include "typedef.h"
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>
// account for trying to put better option into hashmap.
// account for diagonal move out of only first hole.

class SokobanState;
class SokobanMap;

struct normalSokobanStateHash {
    std::size_t operator()(std::shared_ptr<const SokobanState> x) const;
};

struct normalSokobanStateEquals {
    bool operator()(std::shared_ptr<const SokobanState> lhs, std::shared_ptr<const SokobanState> rhs) const;
};

struct aggresiveSokobanStateHash {
    std::size_t operator()(std::shared_ptr<const SokobanState> x) const;
};

struct aggresiveSokobanStateEquals {
    bool operator()(std::shared_ptr<const SokobanState> lhs, std::shared_ptr<const SokobanState> rhs) const;
};

template <typename T>
concept SokobanStateStructure = requires(T a, std::shared_ptr<const SokobanState> toAdd) {
    a.push(toAdd);
};

template <typename T>
concept SokobanMapStructure = requires(T a, std::shared_ptr<const SokobanState> parent, const Pos newCord, const Pos change, uint16_t turnsTaken) {
    { *std::get<0>(a.emplace(std::make_shared<const SokobanState>(parent, newCord, change, turnsTaken))) } -> std::convertible_to<std::shared_ptr<const SokobanState>>;
    { std::get<1>(a.emplace(std::make_shared<const SokobanState>(parent, newCord, change, turnsTaken))) } -> std::convertible_to<bool>;
};

class SokobanState {
public:
    SokobanState(const SokobanMap &gameMap_);
    SokobanState(const std::shared_ptr<const SokobanState> parent, Pos nPos, Pos pushDir, int turnsTaken_);
    bool operator==(const SokobanState &other) const {
        return posId == other.posId && map == other.map;
    }
    template <bool useMap, SokobanStateStructure T, SokobanMapStructure T2, bool removeImpossible>
    static void addMoves(std::shared_ptr<const SokobanState> cState, T &movesToCheck, T2 &usedStates);
    uint16_t calcMinTurns() const;
    static std::vector<bool> updateMap(std::vector<bool> oldMap, Pos nPos, Pos pushDir, myChar &numHoles);
    static myChar legalSpot(Pos pos);
    static SokobanMap emptyMap;
    static SokobanMap &gameMap;
    myChar remaningHoles;
    const std::vector<bool> map;
    const myChar posId;
    static constexpr myChar npos = 255;
    myChar openSpot(Pos pos) const;
    const std::shared_ptr<const SokobanState> parentPtr;
    const uint16_t turnsTaken;
    const std::vector<bool> noBoulder = std::vector<bool>(false, 1);
};

std::ostream &operator<<(std::ostream &out, const SokobanState &toPrint);

bool isPath(const SokobanMap &gameMap, std::vector<bool> map, myChar posId, int holes);

template <bool useMap, SokobanStateStructure T, SokobanMapStructure T2, bool removeImpossible>
void findMoves(myChar npos ,Pos currentCord, std::vector<myChar> &idsChecked, myChar currentId, std::queue<myChar> &toCheck, std::shared_ptr<const SokobanState> cState, T2 &usedStates, T &movesToCheck, const std::vector<bool> &map, const SokobanMap &gameMap, Pos change) {
    const Pos newCord = currentCord + change;
    myChar newId = gameMap.cordToId[newCord];
    if (gameMap.idToCord[newId] != newCord) {
        return;
    }
    if (map[newId]) {
        const Pos boulderPos = newCord + change;
        if (boulderPos == gameMap.holePos || cState->openSpot(boulderPos) != npos) {
            std::shared_ptr<const SokobanState> newState = std::make_shared<const SokobanState>(cState, newCord, change, idsChecked[currentId] + 1);
            if constexpr (useMap) {
                auto placed = usedStates.emplace(newState);
                if (!std::get<1>(placed)) {
                    const SokobanState &prev = **std::get<0>(placed);
                    if (cState->turnsTaken + idsChecked[currentId] + 1 < prev.turnsTaken) {
                        usedStates.erase(std::get<0>(placed));
                        placed = usedStates.emplace(newState);
                    } else {
                        return;
                    }
                }
                newState = *std::get<0>(placed);
            }
            if constexpr (removeImpossible) {
                if (!isPath(gameMap, newState->map, newState->posId, newState->remaningHoles)) {
                    return;
                }
            }
            movesToCheck.push(newState);
        }
        return;
    }
    if (idsChecked[newId] != npos) {
        return;
    }
    idsChecked[newId] = 1 + idsChecked[currentId];
    toCheck.push(newId);
    if (change.y == 0) {
        const myChar rId = cState->openSpot(newCord + Pos(1, 0));
        if (npos != rId && idsChecked[rId] == npos) {
            idsChecked[rId] = 1 + idsChecked[currentId];
            toCheck.push(rId);
        }
        const myChar lId = cState->openSpot(newCord + Pos(-1, 0));
        if (npos != lId && idsChecked[lId] == npos) {
            idsChecked[lId] = 1 + idsChecked[currentId];
            toCheck.push(lId);
        }
    } else {
        const myChar dId = cState->openSpot(newCord + Pos(0, -1));
        if (npos != dId && idsChecked[dId] == npos) {
            idsChecked[dId] = 1 + idsChecked[currentId];
            toCheck.push(dId);
        }
        const myChar uId = cState->openSpot(newCord + Pos(0, 1));
        if (npos != uId && idsChecked[uId] == npos) {
            idsChecked[uId] = 1 + idsChecked[currentId];
            toCheck.push(uId);
        }
    }
}

template <bool useMap, SokobanStateStructure T, SokobanMapStructure T2, bool removeImpossible>
void SokobanState::addMoves(std::shared_ptr<const SokobanState> cState, T &movesToCheck, T2 &usedStates) {
    const std::vector<bool> &map = cState->map;
    const SokobanMap &gameMap = cState->gameMap;
    const myChar posId = cState->posId;
    std::vector<myChar> idsChecked(map.size(), npos);
    idsChecked[posId] = 0;
    std::queue<myChar> toCheck;
    toCheck.push(posId);
    while (!toCheck.empty()) {
        const myChar currentId = toCheck.front();
        toCheck.pop();
        const Pos currentCord = gameMap.idToCord[currentId];
        findMoves<useMap, T, T2, removeImpossible>(npos,currentCord, idsChecked, currentId, toCheck, cState, usedStates, movesToCheck, map, gameMap, Pos(1, 0));
        findMoves<useMap, T, T2, removeImpossible>(npos,currentCord, idsChecked, currentId, toCheck, cState, usedStates, movesToCheck, map, gameMap, Pos(-1, 0));
        findMoves<useMap, T, T2, removeImpossible>(npos,currentCord, idsChecked, currentId, toCheck, cState, usedStates, movesToCheck, map, gameMap, Pos(0, 1));
        findMoves<useMap, T, T2, removeImpossible>(npos,currentCord, idsChecked, currentId, toCheck, cState, usedStates, movesToCheck, map, gameMap, Pos(0, -1));
    }
}

#endif