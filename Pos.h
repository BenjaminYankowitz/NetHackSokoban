// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#ifndef _POS__H__
#define _POS__H__
#include "typedef.h"
#include "HashMap.h"
#include <ostream>
struct Pos{
    constexpr Pos(): y(0), x(0){};
    constexpr Pos(myChar y_, myChar x_): y(y_), x(x_) {}
    inline constexpr  bool operator==(Pos other) const{
        return y==other.y&&x==other.x;
    }
    inline constexpr bool operator!=(Pos other) const{
        return y!=other.y||x!=other.x;
    }
    inline constexpr  Pos& operator+=(Pos other){
        y+=other.y;
        x+=other.x;
        return *this;
    }
    inline constexpr  Pos& operator-=(Pos other){
        y-=other.y;
        x-=other.x;
        return *this;
    }
    inline constexpr  Pos operator-(Pos other) const{
        return Pos(y-other.y,x-other.x);
    }
    inline constexpr  Pos operator+(Pos other) const{
        return Pos(y+other.y,x+other.x);
    }
    myChar y;
    myChar x;
};

template <>
struct MyHash<Pos> {
    constexpr static std::size_t hash(Pos input) {
        return (static_cast<std::size_t>(input.x) << 8) + input.y;
    }
};

inline std::ostream& operator<<(std::ostream& out, Pos ob){
    return out << (int)ob.x << ',' << (int)ob.y;
}



#endif