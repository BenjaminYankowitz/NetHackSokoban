// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#ifndef SOKOBANSTATER__H__
#define SOKOBANSTATER__H__
#include <iostream>
#include "SokobanState.h"
#include <memory>
template <bool>
struct SokobanStateR {
    SokobanStateR(std::shared_ptr<const SokobanState> input);
    std::shared_ptr<const SokobanState>uWrap;
    bool operator<(const SokobanStateR &other) const;
    uint16_t minTurns;
};

template<bool T>
bool SokobanStateR<T>::operator<(const SokobanStateR &other) const {
    return minTurns > other.minTurns;
}

template<bool T>
std::ostream &operator<<(std::ostream &out, const SokobanStateR<T> &sokoState) {
    return out << *sokoState.uWrap;
}

template<>
SokobanStateR<true>::SokobanStateR(std::shared_ptr<const SokobanState>input) : uWrap(input), minTurns(input->turnsTaken+input->calcMinTurns()){}
template<>
SokobanStateR<false>::SokobanStateR(std::shared_ptr<const SokobanState>input) : uWrap(input), minTurns(input->turnsTaken){}


#endif