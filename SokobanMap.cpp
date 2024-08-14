#include "SokobanMap.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <array>
SokobanMap::SokobanMap(const std::string &strMap) {
    holeNumber = 0;
    for(auto i : strMap){
        if(i == '^'){
            holeNumber++;
        }
    }
    std::vector<std::string> tdStrMap;
    std::size_t pos = 0;
    while (true) {
        std::size_t oldPos = pos;
        pos = strMap.find('\n', oldPos + 1);
        if (pos != std::string::npos) {
            tdStrMap.push_back(strMap.substr(oldPos, pos - oldPos));
        } else {
            tdStrMap.push_back(strMap.substr(oldPos));
            break;
        }
        pos++;
    }
    std::vector<Pos> toSeach;
    for (std::size_t y = 0; y < tdStrMap.size(); y++) {
        for (std::size_t x = 0; x < tdStrMap[y].size(); x++) {
            if (tdStrMap[y][x] == '>' || tdStrMap[y][x] == '@') {
                toSeach.push_back(Pos(y, x));
                tdStrMap[y][x] = 'x';
            }
        }
    }
    for (auto i : toSeach) {
        idToCord.push_back(i);
        initStateBolder.push_back(false);
    }
    while (!toSeach.empty()) {
        const Pos cSpot = toSeach.back();
        toSeach.pop_back();
        auto checkMapSpot = [cSpot, &tdStrMap, &toSeach, this](Pos mov) {
            const Pos nPos = cSpot + mov;
            if (tdStrMap[nPos.y][nPos.x] == '.' || tdStrMap[nPos.y][nPos.x] == '0') {
                toSeach.push_back(nPos);
                idToCord.push_back(nPos);
                initStateBolder.push_back(tdStrMap[nPos.y][nPos.x] == '0');
                tdStrMap[nPos.y][nPos.x] = 'x';
            } else if (tdStrMap[nPos.y][nPos.x] == '^') {
                holePos = nPos;
            }
        };
        checkMapSpot(Pos(0, 1));
        checkMapSpot(Pos(0, -1));
        checkMapSpot(Pos(1, 0));
        checkMapSpot(Pos(-1, 0));
    }
    Pos offSet(255, 255);
    for (offSet.y = 0; offSet.y < tdStrMap.size() && tdStrMap[offSet.y].find('x') == std::string::npos; offSet.y++) {
    };
    assert(offSet.y!=tdStrMap.size());
    for (myChar y = offSet.y; y < tdStrMap.size(); y++) {
        std::size_t fPos = tdStrMap[y].find('x');
        if (fPos == std::string::npos) {
            break;
        }
        if (fPos < offSet.x) {
            offSet.x = fPos;
        }
    };
    for (auto &i : idToCord) {
        i -= offSet;
    }
    cordToId = HashMap<Pos, myChar>(idToCord);
    holePos -= offSet;
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (y == 0 && x == 0) {
                continue;
            }
            const Pos nPos = holePos + Pos(y, x);
            const myChar nPosid = cordToId[nPos];
            if (idToCord[nPosid] != nPos) {
                continue;
            }
            outSideHoleId.push_back(nPosid);
        }
    }
    // std::vector<Pos> corners;
    // noComplete = std::vector<bool>(idToCord.size(), false);
    // const std::array<Pos, 4> dirs = {Pos(0, 1), Pos(1, 0), Pos(0, -1), Pos(-1, 0)};
    // for (const auto i : idToCord) {
    //     if (noComplete[cordToId[i]]) {
    //         continue;
    //     }
    //     std::array<bool, dirs.size()> isFree;
    //     for (std::size_t i2 = 0; i2 < dirs.size(); i2++) {
    //         const Pos cSpot = dirs[i2] + i;
    //         isFree[i2] = cSpot == idToCord[cordToId[cSpot]] || cSpot == holePos;
    //     }
    //     for (std::size_t i2 = 0; i2 < isFree.size(); i2++) {
    //         if (!isFree[i2] && !isFree[(i2 + 1) % isFree.size()]) {
    //             noComplete[cordToId[i]] = true;
    //             corners.push_back(i);
    //             break;
    //         }
    //     }
    // }
    // for (const auto i : corners) {
    //     std::vector<std::pair<Pos, Pos>> dirWalCombos;
    //     for (const auto dir : dirs) {
    //         const Pos next = i + dir;
    //         if (idToCord[cordToId[next]] != next) {
    //             continue;
    //         }
    //         const Pos wallD1 = Pos(dir.x, dir.y);
    //         const Pos wallC1 = next + wallD1;
    //         if (idToCord[cordToId[wallC1]] != wallC1) {
    //             dirWalCombos.push_back(std::make_pair(dir, wallD1));
    //         }
    //         const Pos wallD2 = Pos(-dir.x, -dir.y);
    //         const Pos wallC2 = next + wallD1;
    //         if (idToCord[cordToId[wallC2]] != wallC2) {
    //             dirWalCombos.push_back(std::make_pair(dir, wallD2));
    //         }
    //     }
    //     for (auto i2 : dirWalCombos) {
    //         const Pos dir = std::get<0>(i2);
    //         const Pos wallDir = std::get<1>(i2);
    //         Pos cSpot = i;
    //         bool isTrap = true;
    //         while (idToCord[cordToId[cSpot]] == cSpot) {
    //             const Pos wallShould = cSpot + wallDir;
    //             if (idToCord[cordToId[wallShould]] == wallShould && !noComplete[cordToId[wallShould]]) {
    //                 isTrap = false;
    //                 break;
    //             }
    //             cSpot += dir;
    //         }
    //         cSpot = i;
    //         if (isTrap) {
    //             while (idToCord[cordToId[cSpot]] == cSpot) {
    //                 noComplete[cordToId[cSpot]] = true;
    //                 cSpot += dir;
    //             }
    //         }
    //     }
    // }
    distToExit = std::vector<uint16_t>(idToCord.size(),0);
    std::vector<myChar> toCheck;
    std::vector<Pos> cardianalDirs = {Pos(-1,0),Pos(1,0),Pos(0,1),Pos(0,-1)};
    for(Pos i : cardianalDirs){
        const Pos nPos= holePos+i;
        const myChar cId = cordToId[nPos];
        if(idToCord[cId]==nPos){
            toCheck.push_back(cId);
            distToExit[cId] = 1;
        }
    }
    std::vector<myChar> toCheckNext;
    uint16_t cDist = 2;
    while(true){
        if(toCheck.size()==0){
            toCheck = std::move(toCheckNext);
            toCheckNext.clear();
            cDist++;
        }
        if(toCheck.size()==0){
            break;
        }
        myChar cId = toCheck.back();
        toCheck.pop_back();
        auto findMoves = [&toCheckNext, cId, this, cDist](Pos change) {
            const Pos cPos = idToCord[cId];
            const Pos next = cPos+change;
            const Pos back = next+change;
            if(idToCord[cordToId[back]]!=back){
                return;
            }
            const myChar nId = cordToId[next];
            if(idToCord[nId]!=next||distToExit[nId]!=0){
                return;
            }
            distToExit[nId] = cDist;
            toCheckNext.push_back(nId);
            return;
        };
        for(Pos i : cardianalDirs){
            findMoves(i);
        }
    }
}