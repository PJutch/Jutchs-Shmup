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
#include <algorithm>
#include <numbers>
#include <cmath>

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

// WARNING: actually right if rect.width < 0
template <typename T>
T left(sf::Rect<T> rect) noexcept {
    return rect.left;
}

// WARNING: actually left if rect.width < 0
template <typename T>
T right(sf::Rect<T> rect) noexcept {
    return rect.left + rect.width;
}

// WARNING: actually bottom if rect.height < 0
template <typename T>
T top(sf::Rect<T> rect) noexcept {
    return rect.top;
}

// WARNING: actually top if rect.height < 0
template <typename T>
T bottom(sf::Rect<T> rect) noexcept {
    return rect.top + rect.height;
}

// WARNING: doesn't work if rect.width < 0
template <typename T>
sf::Rect<T> extendLeft(sf::Rect<T> rect, T left) noexcept {
    if (rect.left > left) {
        rect.width += rect.left - left;
        rect.left = left;
    }
    return rect;
}

// WARNING: doesn't work if rect.width < 0
template <typename T>
sf::Rect<T> extendRight(sf::Rect<T> rect, T right) noexcept {
    if (rect.right < right) {
        rect.width += right - rect.right;
        rect.right = right;
    }
    return rect;
}

// WARNING: doesn't work if rect.height < 0
template <typename T>
sf::Rect<T> extendTop(sf::Rect<T> rect, T top) noexcept {
    if (rect.top > top) {
        rect.height += rect.top - top;
        rect.top = top;
    }
    return rect;
}

// WARNING: doesn't work if rect.height < 0
template <typename T>
sf::Rect<T> extendBottom(sf::Rect<T> rect, T bottom) noexcept {
    if (rect.bottom < bottom) {
        rect.height += bottom - rect.bottom;
        rect.bottom = bottom;
    }
    return rect;
}

template <typename T>
T dot(sf::Vector2<T> lhs, sf::Vector2<T> rhs) noexcept {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
T length(sf::Vector2<T> vec) noexcept {
    return std::sqrt(dot(vec, vec));
}

template <typename T>
sf::Vector2<T> normalize(sf::Vector2<T> vec) noexcept {
    return vec / length(vec);
}

template <typename T>
T to_deegrees(T angle) noexcept {
    return angle * 180.f / std::numbers::pi_v<float>;
}

#endif