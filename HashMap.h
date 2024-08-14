// Copyright 2024 Benjamin Yankowitz.
// This file is part of NetHack Sokoban Solver.
// NetHack Sokoban Solver is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3 of the License.
// NetHack Sokoban Solver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>.

#ifndef _HASHMAP__H__
#define _HASHMAP__H__
#include <cstdint>
#include <vector>
template <class T>
struct MyHash {};

template <>
struct MyHash<uint16_t> {
    constexpr static std::size_t hash(uint16_t input) {
        return input;
    }
};

template <typename T>
concept Hashable = requires(T a) {
    { MyHash<T>::hash(a) } -> std::convertible_to<std::size_t>;
};

template <Hashable Key_t>
constexpr std::size_t getHashMapLen(const std::vector<Key_t> &input) {
    bool done = false;
    std::size_t len = input.size() - 1;
    while (!done) {
        done = true;
        len++;
        std::vector<bool> used(len, false);
        for (const Key_t i : input) {
            std::size_t index = MyHash<Key_t>::hash(i) % len;
            if (used[index]) {
                done = false;
                break;
            }
            used[index] = true;
        }
    }
    return len;
}

template <Hashable Key_t, class Value_t>
std::vector<Value_t> getHashMapMap(std::size_t mapSize, const std::vector<Key_t> &input) {
    std::vector<Value_t> map(mapSize, 0);
    for (std::size_t i = 0; i < input.size(); i++) {
        map[MyHash<Key_t>::hash(input[i]) % mapSize] = i;
    }
    return map;
}

template <Hashable Key_t, class Value_t>
struct HashMap {
    HashMap() {}
    HashMap(const std::vector<Key_t> &input) : mapSize(getHashMapLen<Key_t>(input)), map(getHashMapMap<Key_t, Value_t>(mapSize, input)) {}
    Value_t operator[](Key_t inV) const {
        return map[MyHash<Key_t>::hash(inV) % mapSize];
    }
    std::size_t mapSize;
    std::vector<Value_t> map;
};

#endif