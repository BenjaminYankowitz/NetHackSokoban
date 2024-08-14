// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#ifndef SOKOBANSOLVER__H__
#define SOKOBANSOLVER__H__

#include "SokobanMap.h"
#include "SokobanState.h"
#include "memory"


template <bool useAstar>
std::pair<std::vector<Pos>,std::size_t> astar(const SokobanMap& gameMap);
std::pair<std::vector<Pos>,std::size_t> breadthFirstSearch(const SokobanMap& gameMap);
std::pair<std::vector<Pos>,std::size_t> depthFirstSearch(const SokobanMap& gameMap);
// std::pair<std::vector<Pos>,std::size_t> depthFirstSearchwithIterativeDeepining(const SokobanMap& gameMap);


#include "SokobanState.h"
#include "SokobanStateR.h"
#include <iostream>
#include <stack>


// typedef std::unordered_set<std::shared_ptr<const SokobanState>,aggresiveSokobanStateHash,aggresiveSokobanStateEquals> aggresive_hashmap;
typedef std::unordered_set<std::shared_ptr<const SokobanState>,normalSokobanStateHash,normalSokobanStateEquals> normal_hashmap;

constexpr bool printAstar = false;
constexpr bool useHashMap = false;
constexpr bool useMoveCull = true;

std::vector<Pos> constructSequence(const std::shared_ptr<const SokobanState> finalState){
    std::vector<Pos> ret(finalState->turnsTaken+1);
    std::shared_ptr<const SokobanState> cStatePtr = finalState;
    while(cStatePtr!=nullptr){
        const SokobanState& cState = *cStatePtr;
        ret[cState.turnsTaken] = cState.gameMap.idToCord[cState.posId];
        cStatePtr = cState.parentPtr;
    }
    return ret;
}

template <bool useAstar>
std::pair<std::vector<Pos>,std::size_t> astar(const SokobanMap& gameMap){
    normal_hashmap usedStates;
    typedef std::priority_queue<SokobanStateR<useAstar>> QT;
    QT movesToCheck;
    if constexpr(useHashMap){
        auto firstState = usedStates.emplace(std::make_shared<const SokobanState>(gameMap));
        movesToCheck.emplace(*std::get<0>(firstState));
    } else {
        movesToCheck.emplace(std::make_shared<const SokobanState>(gameMap));
    }
    uint16_t bestVal = 65535;
    std::shared_ptr<const SokobanState> finalState = nullptr;
    int totalStates = 1;
    while (!movesToCheck.empty()) {
        SokobanStateR<useAstar> mToCheck = movesToCheck.top();
        movesToCheck.pop();
        totalStates++;
        std::shared_ptr<const SokobanState> nState = mToCheck.uWrap;
        if(nState->remaningHoles==0){
            finalState = mToCheck.uWrap;
            break;
        }
        if constexpr (useHashMap){
            std::shared_ptr<const SokobanState> prevState = *usedStates.find(mToCheck.uWrap); 
            if(prevState->turnsTaken<mToCheck.uWrap->turnsTaken){
                continue;
            }
        }
        SokobanState::addMoves<useHashMap,QT,normal_hashmap,useMoveCull>(nState,movesToCheck,usedStates);
        if constexpr (useAstar&&printAstar){
            auto& output = std::cout;
            if (mToCheck.minTurns - nState->turnsTaken < bestVal) {
                bestVal = mToCheck.minTurns - nState->turnsTaken;
                output << "mtCheck:\n" << mToCheck << '\n';
                output << "bestVal: " << (int)bestVal << '\n';
                output << "remaining holes: " << (int)nState->remaningHoles << '\n';
            }
        }
    }
    totalStates+=movesToCheck.size();
    if constexpr (useHashMap){
        totalStates = usedStates.size();
    }
    return std::make_pair(constructSequence(finalState),totalStates);
}

std::pair<std::vector<Pos>,std::size_t> breadthFirstSearch(const SokobanMap& gameMap){
    // std::ofstream output("out.txt");
    // auto& output = std::cout;
    normal_hashmap usedStates;
    typedef std::queue<std::shared_ptr<const SokobanState>> QT;
    QT movesToCheck;
    int totalStates = 1;
    if constexpr(useHashMap){
        auto firstState = usedStates.emplace(std::make_shared<const SokobanState>(gameMap));
        movesToCheck.push(*std::get<0>(firstState));
    } else {
        movesToCheck.push(std::make_shared<const SokobanState>(gameMap));
    }
    std::shared_ptr<const SokobanState> finalState = nullptr;
    while (!movesToCheck.empty()) {
        totalStates++;
        std::shared_ptr<const SokobanState> mToCheck = movesToCheck.front();
        movesToCheck.pop();
        if constexpr (useHashMap){
            if((*usedStates.find(mToCheck))->turnsTaken<mToCheck->turnsTaken){
                continue;
            }
        }
        if(mToCheck->remaningHoles==0){
            finalState = mToCheck;
            break;
        }
        SokobanState::addMoves<useHashMap,QT,normal_hashmap,useMoveCull>(mToCheck,movesToCheck, usedStates);
    }
    totalStates+=movesToCheck.size();
    if constexpr (useHashMap){
        totalStates = usedStates.size();
    }
    return std::make_pair(constructSequence(finalState),totalStates);
}

std::pair<std::vector<Pos>,std::size_t> depthFirstSearch(const SokobanMap& gameMap){
    normal_hashmap usedStates;
    typedef std::stack<std::shared_ptr<const SokobanState>> QT;
    QT movesToCheck;
    int totalStates = 1;
    if constexpr (useHashMap){
        auto firstState = usedStates.emplace(std::make_shared<const SokobanState>(gameMap));
        movesToCheck.emplace(*std::get<0>(firstState));
    } else {
        movesToCheck.emplace(std::make_shared<const SokobanState>(gameMap));
    }
    uint16_t turnsTaken = 65535;
    while (!movesToCheck.empty()) {
        totalStates++;
        std::shared_ptr<const SokobanState> mToCheck = movesToCheck.top();
        movesToCheck.pop();
        if(mToCheck->remaningHoles==0){
            turnsTaken = mToCheck->turnsTaken;
            break;
        }
        SokobanState::addMoves<useHashMap,QT,normal_hashmap,useMoveCull>(mToCheck,movesToCheck, usedStates);
    }
    totalStates+=movesToCheck.size();
    if constexpr (useHashMap){
        totalStates = usedStates.size();
    }
    return std::make_pair(std::vector<Pos>(turnsTaken),totalStates);
}

// std::vector<Pos> depthFirstSearchwithIterativeDeepining(const SokobanMap& gameMap){
//     uint16_t turnsTaken = 65535;
//     uint16_t depth = 0;
//     while(turnsTaken==65535){
//         depth+=50;
//         // std::cout << depth << '\n';
//         FakeHashMap usedStates;
//         std::stack<const SokobanState*> movesToCheck;
//         auto firstState = usedStates.emplace(gameMap);
//         movesToCheck.push(&*std::get<0>(firstState));
//         while (!movesToCheck.empty()) {
//             const SokobanState& mToCheck = *movesToCheck.top();
//             movesToCheck.pop();
//             if(mToCheck.turnsTaken<depth){
//                 continue;
//             }
//             mToCheck.addMoves<true,std::stack<const SokobanState*>,FakeHashMap>(movesToCheck, usedStates);
//             if(mToCheck.remaningHoles==0){
//                 turnsTaken = mToCheck.turnsTaken;
//                 return std::vector<Pos>(turnsTaken);
//                 break;
//             }
//         }
//     }
//     return std::vector<Pos>(turnsTaken);
// }
#endif