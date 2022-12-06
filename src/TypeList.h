/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef TYPE_LIST_H_
#define TYPE_LIST_H_

#include <concepts>

struct Empty {};

template <typename... Ts>
struct TypeList {};

template <typename T, typename... Ts>
struct TypeList<T, Ts...> {};

namespace detail {
    template <typename TList>
    struct head {
        using type = Empty;
    };

    template <typename T, typename... Ts>
    struct head<TypeList<T, Ts...>> {
        using type = T;
    };
}
template <typename TList>
using head_t = typename detail::head<TList>::type;

namespace detail {
    template <typename TList>
    struct tail {
        using type = Empty;
    };

    template <typename T, typename... Ts>
    struct tail<TypeList<T, Ts...>> {
        using type = TypeList<Ts...>;
    };
}
template <typename TList>
using tail_t = typename detail::tail<TList>::type;

namespace detail {
    template <typename T, typename TList>
    struct pushFront;

    template <typename T, typename... Ts>
    struct pushFront<T, TypeList<Ts...>> {
        using type = TypeList<T, Ts...>;
    };
}
template <typename T, typename TList>
using pushFront_t = typename detail::pushFront<T, TList>::type;

#endif