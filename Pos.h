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