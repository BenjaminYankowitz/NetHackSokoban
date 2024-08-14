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