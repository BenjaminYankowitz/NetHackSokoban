#include "SokobanState.h"
#include "SokobanMap.h"
#include <iostream>
#include <stack>
#include <limits>
//A*
// Multiple different value functions.
//Breadth First search
//Why am I chosing the ones i'm chosing.
//Uniform cost search.
//Depth first search with iterative deepning.
//Hallway detection? Maybe other optimizations (can be applied to multiple different solutions).

bool isConnected(const SokobanState& state1, const SokobanState& state2){
    std::vector<bool> idsChecked(state1.gameMap.initStateBolder.size(),false);
    idsChecked[state1.posId] = true;
    std::vector<myChar> toCheck = {state1.posId};
    std::vector<myChar> toCheckNext;
    uint16_t cDist = 1;
    Pos goalPos = state2.gameMap.idToCord[state2.posId];
    while(true){
        if(toCheck.size()==0){
            if(toCheckNext.size()==0){
                return false;
            }
            toCheck = std::move(toCheckNext);
            toCheckNext.clear();
            cDist++;
        }
        myChar cId = toCheck.back();
        toCheck.pop_back();
        auto findMoves = [state2, cId, &idsChecked, &toCheckNext,goalPos](Pos change) {
            const Pos cPos = state2.gameMap.idToCord[cId];
            const Pos next = cPos+change;
            if(next==goalPos){
                return true;
            }
            const myChar nId = state2.gameMap.cordToId[next];
            if(state2.gameMap.idToCord[nId]!=next){
                return false;
            }
            if(idsChecked[nId]){
                return false;
            }
            idsChecked[nId] = true;
            toCheckNext.push_back(nId);
            return false;
        };
        if(findMoves(Pos(-1,0))||findMoves(Pos(1,0))||findMoves(Pos(0,-1))||findMoves(Pos(0,1))){
            return cDist;
        }
    }
}

std::size_t normalSokobanStateHash::operator()(std::shared_ptr<const SokobanState>x) const {
    return std::hash<std::vector<bool>>{}(x->map) + x->posId*7187;
}
bool normalSokobanStateEquals::operator()(std::shared_ptr<const SokobanState>lhs,std::shared_ptr<const SokobanState>rhs) const{
    return (*lhs)==(*rhs);
}

std::size_t aggresiveSokobanStateHash::operator()(std::shared_ptr<const SokobanState>x) const{
    return std::hash<std::vector<bool>>{}(x->map);
}

bool aggresiveSokobanStateEquals::operator()(std::shared_ptr<const SokobanState>lhs,std::shared_ptr<const SokobanState>rhs) const{
    if(lhs->map!=rhs->map){
        return false;
    }
    return isConnected(*lhs,*rhs);
}


SokobanState::SokobanState(const SokobanMap &gameMap_) : remaningHoles(gameMap_.holeNumber), map(gameMap_.initStateBolder), posId(0), parentPtr(nullptr), turnsTaken(0) {
    gameMap = gameMap_;
}
SokobanState::SokobanState(const std::shared_ptr<const SokobanState> parent, Pos nPos, Pos pushDir, int turnsTaken_) : remaningHoles(parent->remaningHoles), map(updateMap(parent->map, nPos, pushDir, remaningHoles)), posId(gameMap.cordToId[nPos]), parentPtr(parent), turnsTaken(turnsTaken_ + parent->turnsTaken){};

SokobanMap SokobanState::emptyMap = SokobanMap();
SokobanMap &SokobanState::gameMap = SokobanState::emptyMap;

myChar SokobanState::openSpot(Pos pos) const {
    myChar ret = gameMap.cordToId[pos];
    if (gameMap.idToCord[ret] != pos || map[ret]) {
        return npos;
    } else {
        return ret;
    }
}

myChar SokobanState::legalSpot(Pos pos) {
    myChar ret = gameMap.cordToId[pos];
    if (gameMap.idToCord[ret] != pos) {
        return npos;
    } else {
        return ret;
    }
}

bool isPath(const SokobanMap &gameMap, std::vector<bool> map, myChar posId,int holes) {
    for(std::size_t i = 0; i < map.size(); i++){
        if(map[i]&&gameMap.distToExit[i]!=0){
            holes--;
        }
    }
    if(holes>0){
        return false;
    }
    std::vector<myChar> toCheck;
    std::vector<bool> idsChecked(map.size(), false);
    toCheck.push_back(posId);
    idsChecked[posId] = true;
    std::vector<myChar> boulders;
    while (!toCheck.empty()) {
        const myChar cId = toCheck.back();
        toCheck.pop_back();
        const Pos cPos = gameMap.idToCord[cId];
        auto addVal = [cPos, &toCheck, &gameMap, &map, &idsChecked, &boulders, cId](Pos dir) {
            const Pos uPos = cPos + dir;
            if (uPos == gameMap.holePos) {
                return true;
            }
            const myChar uId = gameMap.cordToId[uPos];
            if (idsChecked[uId] || gameMap.idToCord[uId] != uPos) {
                return false;
            }
            if (map[uId]) {
                const Pos bPos = uPos + dir;
                if (bPos == gameMap.holePos) {
                    return true;
                }
                const myChar bId = gameMap.cordToId[bPos];
                if (gameMap.idToCord[bId] != bPos) {
                    return false;
                }
                if (map[bId]) {
                    boulders.push_back(cId);
                    return false;
                }
                map[uId] = false;
                for (auto i : boulders) {
                    toCheck.push_back(i);
                }
                boulders.clear();
            }
            toCheck.push_back(uId);
            idsChecked[uId] = true;
            return false;
        };
        if (addVal(Pos(1, 0)) || addVal(Pos(-1, 0)) || addVal(Pos(0, 1)) || addVal(Pos(0, -1))) {
            return true;
        }
    }
    return false;
}

// To get this to work have to implement getting distance and checking for getting to other sides.
// uint16_t distToExit(const SokobanMap& gameMap, myChar cId, const std::vector<bool> map){
//     const Pos holePos = gameMap.holePos;
//     std::vector<bool> idsChecked(gameMap.idToCord.size(),false);
//     idsChecked[cId] = true;
//     std::vector<myChar> toCheck = {cId};
//     std::vector<myChar> toCheckNext;
//     uint16_t cDist = 1;
//     while(true){
//         if(toCheck.size()==0){
//             toCheck = std::move(toCheckNext);
//             toCheckNext.clear();
//             cDist++;
//         }
//         if(toCheck.size()==0){
//             return std::numeric_limits<uint16_t>::max();
//         }
//         myChar cId = toCheck.back();
//         toCheck.pop_back();
//         auto findMoves = [holePos, cId, &idsChecked, &toCheckNext, &gameMap, &map](Pos change) {
//             const Pos cPos = gameMap.idToCord[cId];
//             const Pos back = cPos-change;
//             const Pos next = cPos+change;
//             myChar backId = gameMap.cordToId[back];
//             if(gameMap.idToCord[backId]!=back||(backId!=cId&&map[backId])){
//                 return false;
//             }
//             if(next==holePos){
//                 return true;
//             }
//             const myChar nId = gameMap.cordToId[next];
//             if(gameMap.idToCord[nId]!=next){
//                 return false;
//             }
//             if(idsChecked[nId]){
//                 return false;
//             }
//             idsChecked[nId] = true;
//             toCheckNext.push_back(nId);
//             return false;
//         };
//         if(findMoves(Pos(-1,0))||findMoves(Pos(1,0))||findMoves(Pos(0,-1))||findMoves(Pos(0,1))){
//             return cDist;
//         }
//     }
// }


uint16_t SokobanState::calcMinTurns() const {
    // std::cout << "rH: " << remaningHoles << '\n';
    if (remaningHoles == 0) {
        return 0;
    }
    myChar holesLeft = remaningHoles;
    std::vector<bool> mapC(map);
    uint16_t answer = 0;
    while (holesLeft > 0) {
        myChar cDist = 1;
        std::vector<bool> idsChecked(map.size(), false);
        std::vector<myChar> toCheck;
        std::vector<myChar> toCheckNext;
        if (answer == 0) {
            idsChecked[posId] = true;
            toCheck.push_back(posId);
        } else {
            for (myChar i : gameMap.outSideHoleId) {
                idsChecked[i] = true;
                toCheck.push_back(i);
            }
        }
        while (true) {
            if (toCheck.empty()) {
                std::swap(toCheck, toCheckNext);
                ++cDist;
                if (toCheck.empty()) {
                    return 10000;
                }
            }
            const myChar currentId = toCheck.back();
            toCheck.pop_back();
            const Pos currentCord = gameMap.idToCord[currentId];
            auto findMoves = [currentCord, &idsChecked, &toCheckNext, &mapC, &answer, &holesLeft, cDist](Pos change) {
                auto isFull = [&mapC](Pos pos) {
                    if (pos == gameMap.holePos) {
                        return false;
                    }
                    myChar ret = gameMap.cordToId[pos];
                    if (gameMap.idToCord[ret] != pos || mapC[ret]) {
                        return true;
                    } else {
                        return false;
                    }
                };
                if (holesLeft == 0) {
                    return true;
                }
                const Pos newCord = currentCord + change;
                myChar newId = gameMap.cordToId[newCord];
                if (gameMap.idToCord[newId] != newCord || idsChecked[newId]) {
                    return false;
                }
                idsChecked[newId] = true;
                if (mapC[newId]) {
                    if (gameMap.distToExit[newId]==0) {
                        return false;
                    }
                    if (isFull(newCord + change)) {
                        idsChecked[newId] = false;
                        return false;
                    }
                    answer += cDist+gameMap.distToExit[newId];
                    --holesLeft;
                    if (holesLeft == 0) {
                        return true;
                    }
                    mapC[newId] = false;
                    return true;
                }
                toCheckNext.push_back(newId);
                if (change.y == 0) {
                    const myChar rId = legalSpot(newCord + Pos(1, 0));
                    if (npos != rId && !idsChecked[rId] && !mapC[rId]) {
                        idsChecked[rId] = true;
                        toCheckNext.push_back(rId);
                    }
                    const myChar lId = legalSpot(newCord + Pos(-1, 0));
                    if (npos != lId && !idsChecked[lId] && !mapC[lId]) {
                        idsChecked[lId] = true;
                        toCheckNext.push_back(lId);
                    }
                } else {
                    const myChar dId = legalSpot(newCord + Pos(0, -1));
                    if (npos != dId && !idsChecked[dId] && !mapC[dId]) {
                        idsChecked[dId] = true;
                        toCheckNext.push_back(dId);
                    }
                    const myChar uId = legalSpot(newCord + Pos(0, 1));
                    if (npos != uId && !idsChecked[uId] && !mapC[uId]) {
                        idsChecked[uId] = true;
                        toCheckNext.push_back(uId);
                    }
                }
                return false;
            };
            if (findMoves(Pos(1, 0)) || findMoves(Pos(-1, 0)) || findMoves(Pos(0, 1)) || findMoves(Pos(0, -1))) {
                break;
            }
        }
    }
    return answer;
}
std::vector<bool> SokobanState::updateMap(std::vector<bool> map, Pos nPos, Pos pushDir, myChar &numHoles) {
    const Pos boulderPos = nPos + pushDir;
    myChar nSpotid = gameMap.cordToId[nPos];
    map[nSpotid] = false;
    if (boulderPos != gameMap.holePos) {
        map[gameMap.cordToId[boulderPos]] = true;
    } else {
        --numHoles;
    }
    return map;
}

std::ostream &operator<<(std::ostream &out, const SokobanState &toPrint) {
    Pos downRightP(0, 0);
    for (auto i : toPrint.gameMap.idToCord) {
        if (i.x > downRightP.x) {
            downRightP.x = i.x;
        }
        if (i.y > downRightP.y) {
            downRightP.y = i.y;
        }
    }
    for (int i = 0; i <= downRightP.x + 2; i++) {
        out << '-';
    }
    out << '\n';
    for (int y = 0; y <= downRightP.y; y++) {
        out << '|';
        for (int x = 0; x <= downRightP.x; x++) {
            const Pos cPos(y, x);
            const myChar id = toPrint.legalSpot(cPos);
            if (id == toPrint.npos) {
                if (cPos == toPrint.gameMap.holePos) {
                    out << '^';
                    continue;
                }
                int sideScore = (toPrint.legalSpot(cPos + Pos(1, 0)) != toPrint.npos) + (toPrint.legalSpot(cPos + Pos(-1, 0)) != toPrint.npos);
                int tallScroe = (toPrint.legalSpot(cPos + Pos(0, 1)) != toPrint.npos) + (toPrint.legalSpot(cPos + Pos(0, -1)) != toPrint.npos);
                int totalScore = sideScore + tallScroe + (toPrint.legalSpot(cPos + Pos(1, 1)) != toPrint.npos) + (toPrint.legalSpot(cPos + Pos(-1, 1)) != toPrint.npos) + (toPrint.legalSpot(cPos + Pos(1, -1)) != toPrint.npos) + (toPrint.legalSpot(cPos + Pos(-1, -1)) != toPrint.npos);
                if (sideScore >= tallScroe && totalScore > 0) {
                    out << '-';
                } else if (totalScore > 0) {
                    out << "|";
                } else {
                    out << ' ';
                }
            } else if (toPrint.map[id]) {
                out << '0';
            } else if (id == toPrint.posId) {
                out << '@';
            } else if (id == 0) {
                out << '>';
            } else {
                out << '.';
            }
        }
        out << "|\n";
    }
    for (int i = 0; i <= downRightP.x + 2; i++) {
        out << '-';
    }
    return out;
}