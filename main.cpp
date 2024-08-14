// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#include "HashMap.h"
#include "SokobanMap.h"
#include "SokobanSolvers.h"
#include <iostream>
#include <limits>
#include <chrono>
// typedef HashMap<uint16_t, char> Map;

std::string secondsTaken(std::pair<std::vector<Pos>,std::size_t> (*func)(const SokobanMap&), const SokobanMap& map){
    const auto start{std::chrono::steady_clock::now()};
    auto answer = func(map);
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double, std::ratio<1,1000>> elapsed_seconds = end - start;
    ulong totalSeconds = elapsed_seconds.count();
    // int seconds = totalSeconds%60;
    // totalSeconds/=60;
    // int minutes = totalSeconds%60;
    // totalSeconds/=60;
    // int hours = totalSeconds;
    std::string ret = std::to_string(std::get<0>(answer).size()-1);
    while(ret.size()<8){
        ret+=' ';
    }
    ret+= "& ";
    ret+=std::to_string(std::get<1>(answer));
    while(ret.size()<22){
        ret+=' ';
    }
    ret+= "& ";
    ret+=std::to_string(totalSeconds);
    // bool started = false;
    // if(started || hours != 0){
    //     started = true;
    //     ret += std::to_string(hours) + " hr";
    // }
    // if(started || minutes != 0){
    //     if(started){
    //         ret += ' ';
    //     }
    //     started = true;
    //     ret += std::to_string(minutes) + " min";
    // }
    // if(started){
    //     ret += ' ';
    // }
    // ret += std::to_string(seconds) + "s";
    return ret;
}

void printVec(std::vector<Pos> vec){
    if(vec.size()==0){
        std::cout << "[]\n";
    }
    std::cout << "[(";
    std::cout << vec[0] << ")";
    for(std::size_t i = 1; i < vec.size();i++){
        std::cout << ",(" << vec[i] << ')';
    }
    std::cout << "]\n";
}

int main() {
    // const std::vector<uint16_t> input = {0, 1, 2, 3, 7, 8, 9, 10, 12, 13};
    // const Map map(input);
    const std::vector<std::string> gameMapArr = {
        "\
-------- ------\n\
|<|>...---....|\n\
|.|-..........|\n\
|.||..0.|.....|\n\
|.||....|.....|\n\
|.|------.----|\n\
|.|    |......|\n\
|.------......|\n\
|.....^.......|\n\
|..-----......|\n\
----   --------",
        "\
-------- ------\n\
|<|>...---....|\n\
|.|-..........|\n\
|.||..00|.....|\n\
|.||....|.....|\n\
|.|------0----|\n\
|.|    |......|\n\
|^------......|\n\
|..^^^^0.0....|\n\
|..-----......|\n\
----   --------",
        "\
-------- ------\n\
|<|>...---....|\n\
|.|-..........|\n\
|.||..00|...0.|\n\
|.||....|.....|\n\
|^|------.----|\n\
|^|    |......|\n\
|^------......|\n\
|..^^^^0000...|\n\
|..-----......|\n\
----   --------",
//         "\
// -------- ------\n\
// |<|>...---....|\n\
// |^|-.00....0..|\n\
// |^||..00|.0.0.|\n\
// |^||....|.....|\n\
// |^|------0----|\n\
// |^|    |......|\n\
// |^------......|\n\
// |..^^^^0000...|\n\
// |..-----......|\n\
// ----   --------",
// "\
// ------  ----- \n\
// |....|  |...| \n\
// |.0..----.0.| \n\
// |.0......0..| \n\
// |..--->---0.| \n\
// |---------.---\n\
// |..^^^<|.....|\n\
// |..----|0....|\n\
// --^|   |.0...|\n\
//  |^-----.0...|\n\
//  |..^^^^0.0..|\n\
//  |..----------\n\
//  ----         ",

// "\
//  ----          -----------\n\
// --.>--------   |.........|\n\
// |..........|   |.........|\n\
// |.0-----0-.|   |.........|\n\
// |..|...|.0.|   |....<....|\n\
// |.0.0....0-|   |.........|\n\
// |.0..0..|..|   |.........|\n\
// |.----0.--.|   |.........|\n\
// |..0...0.|.--  |.........|\n\
// |.---0-...0.------------+|\n\
// |...|..0-.0.^^^^^^^^^^^^.|\n\
// |..0......----------------\n\
// -----..|..|               \n\
//     -------               ",
// "\
// -----------       -----------\n\
// |....|....---     |.........|\n\
// |..00|00...>|     |.........|\n\
// |.....0...---     |.........|\n\
// |....|....|       |....<....|\n\
// |-.---------      |.........|\n\
// |..0.|.....|      |.........|\n\
// |.00.|0.0.0|      |.........|\n\
// |..0.....0.|      |.........|\n\
// |.000|0..0.----------------+|\n\
// |....|..0.0.^^^^^^^^^^^^^^^.|\n\
// -----------------------------",

// "\
//   --------          \n\
// ---.|....|          \n\
// |...0....|----------\n\
// |.-.00-00|.|.......|\n\
// |.00-......|.......|\n\
// |.-..0.|...|.......|\n\
// |....-0--0-|...<...|\n\
// |..00..0...|.......|\n\
// |.--...|...|.......|\n\
// |....-0|---|.......|\n\
// ---..0.-----------+|\n\
//   |..0>^^^^^^^^^^^.|\n\
//   ------------------",
   
// "\
// --------------------\n\
// |........|...|.....|\n\
// |.00..-00|.-.|.....|\n\
// |..|.0.0.|00.|.....|\n\
// |-.|..-..|.-.|..<..|\n\
// |...--.......|.....|\n\
// |...|.0.-...-|.....|\n\
// |.0.|0.|...--|.....|\n\
// |-0.|..-----------+|\n\
// |..0....^^^^^^^^^^.|\n\
// |...|.>-------------\n\
// --------            ",

// "\
// --------------------------\n\
// >......^^^^^^^^^^^^^^^^.|\n\
// |.......----------------.|\n\
// -------.------         |.|\n\
//  |...........|         |.|\n\
//  |.0.0.0.0.0.|         |.|\n\
// --------.----|         |.|\n\
// |...0.0..0.0.|         |.|\n\
// |...0........|         |.|\n\
// -----.--------   ------|.|\n\
//  |..0.0.0...|  --|.....|.|\n\
//  |.....0....|  |.+.....|.|\n\
//  |.0.0...0.--  |-|.....|.|\n\
// -------.----   |.+.....+.|\n\
// |..0.....|     |-|.....|--\n\
// |........|     |.+.....|  \n\
// |...------     --|.....|  \n\
// -----            -------  "

// "\
//   ------------------------\n\
//   |..^^^^^^^^^^^^^^^^^^..|\n\
//   |..-------------------.|\n\
// ----.|    -----        |.|\n\
// |..|0--  --...|        |.|\n\
// |.....|--|.0..|        |.|\n\
// |.00..|..|..0.|        |.|\n\
// --..00|...00.--        |.|\n\
//  |0..0...|0..|   ------|.|\n\
//  |.00.|..|..0| --|.....|.|\n\
//  |.0.0---|.0.| |.+.....|.|\n\
//  |.......|..-- |-|.....|.|\n\
//  ----.0..|.--  |.+.....+.|\n\
//     ---.--.|   |-|.....|--\n\
//      |.0...|   |.+.....|  \n\
//      |>.|..|   --|.....|  \n\
//      -------     -------  "
};
    // std::cout << gameMapChar << '\n';
    // std::cout << gameMap.idToCord.size() << '\n';
    // std::cout << gameMap.cordToId.mapSize << '\n';
    // printVec(std::get<0>(astar<false>(SokobanMap(gameMapArr[1]))));
    for(const std::string& gameMapChar : gameMapArr){
        SokobanMap gameMap(gameMapChar);
        std::cout << gameMap << '\n';
        const std::string eString = "\\\\\n"; 
        std::cout << "                     & turns   & states      & time (ms)" << eString;
        std::cout << "Depth First Search   & " << secondsTaken(depthFirstSearch,gameMap) << eString;
        std::cout << "A*                   & " << secondsTaken(astar<true>,gameMap) << eString;
        std::cout << "Breadth First Search & " << secondsTaken(breadthFirstSearch,gameMap) << eString;
        std::cout << "Uniform Cost Search  & " << secondsTaken(astar<false>,gameMap) << eString;
    }
}