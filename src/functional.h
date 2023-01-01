/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef FUNCTIONAL_H_
#define FUNCTIONAL_H_

#include <iterator>

template <typename Func>
auto indirect(Func&& func) {
    return [func = std::forward<Func>(func)](std::indirectly_readable auto&... ptr) {
        return std::invoke(func, *ptr...);
    };
}

template <typename Func>
auto not_pred(Func&& func) {
    return [func = std::forward<Func>(func)]<typename... Args>(Args&&... args) {
        return !std::invoke(func, std::forward<Args>(args)...);
    };
}

template <typename T>
auto less_than(T&& val) {
    return [val = std::forward<T>(val)](const auto& other) {
        return other < val;
    };
}

template <typename T>
auto greater_than(T&& val) {
    return [val = std::forward<T>(val)](const auto& other) {
        return other > val;
    };
}

template <typename Component>
const auto makeUniqueFunctor = []<typename... Args>(Args&&... args) 
        requires std::constructible_from<Component, Args...> {
    return std::make_unique<Component>(std::forward<Args>(args)...);
};

#endif