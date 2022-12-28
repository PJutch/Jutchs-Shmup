/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <optional>

// WARNING: always return false if min >= max
template <typename T>
bool between(T value, T min, T max) {
    return min < value && value < max;
}

// WARNING: always return false if min > max
template <typename T>
bool between_nonstrict(T value, T min, T max) {
    return min <= value && value <= max;
}

// WARNING: may return wrong answer if min1 >= max1 || min2 >= max2 
template <typename T>
bool intersects(T min1, T max1, T min2, T max2) {
    return min1 < max2 && min2 < max1;
}

// WARNING: may return wrong answer if min1 > max1 || min2 > max2 
template <typename T>
bool intersects_nonstrict(T min1, T max1, T min2, T max2) {
    return min1 <= max2 && min2 <= max1;
}

// safe if size is negative or infinite
// strict
template <typename T>
inline bool intersects(sf::Rect<T> lhs, sf::Rect<T> rhs) noexcept {
    return lhs.intersects(rhs);
}

// safe if size is negative or infinite
// size > 0 if intersection exists
template <typename T>
inline std::optional<sf::Rect<T>> intersection(sf::Rect<T> lhs, sf::Rect<T> rhs) noexcept {
    sf::Rect<T> res;
    if (lhs.intersects(rhs, res)) {
        return res;
    } else {
        return std::nullopt;
    }
}

// WARNING: actually right if width < 0
template <typename T>
T left(sf::Rect<T> rect) noexcept {
    return rect.left;
}

// WARNING: actually left if width < 0
template <typename T>
T right(sf::Rect<T> rect) noexcept {
    return rect.left + rect.width;
}

// WARNING: actually bottom if height < 0
template <typename T>
T top(sf::Rect<T> rect) noexcept {
    return rect.top;
}

// WARNING: actually top if height < 0
template <typename T>
T bottom(sf::Rect<T> rect) noexcept {
    return rect.top + rect.height;
}

#endif