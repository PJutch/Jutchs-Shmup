/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef REDUCE_H_
#define REDUCE_H_

#include "TypeList.h"

#include <numeric>
#include <execution>
#include <ranges>
#include <iterator>
#include <concepts>
#include <type_traits>

template <typename T>
concept ExecutionPolicy = std::is_execution_policy_v<std::remove_cvref_t<T>>;

/* namespace detail {
    template <typename Operation, typename Result, typename T, 
              typename T2ListToCheck, typename T2List, typename TestedList>                  
    struct isReducable {
        using Result1 = std::invoke_result<Operation, T, head_t<T2ListToCheck>>;
        using Result2 = std::invoke_result<Operation, T, head_t<T2ListToCheck>>;
        using newT2 = pushFront_t<T, T2List>;
        using newTested = pushFront_t<T, TestedList>;

        inline static constexpr bool value = std::convertible_to<T, Result>
            && std::regular_invocable<Operation, T, head_t<T2ListToCheck>>
            && std::regular_invocable<Operation, head_t<T2ListToCheck>, T>
            && isReducable<Operation, Result, Result1, newT2, newT2, TestedList>::value
            && isReducable<Operation, Result, Result1, newT2, newT2>::value
            && isReducable<Operation, Result, T, tail_t<T2ListToCheck>, T2List>::value;
    };

    template <typename Operation, typename Result, typename T, typename T2List, typename TestedList> 
    struct isReducable<Operation, Result, T, Empty, T2List, TestedList> {
        inline static constexpr bool value = std::convertible_to<T, Result>;
    };

    template <typename Operation, typename Result, std::convertible_to<Result> T, 
        typename T2ListToCheck, typename T2List, typename TestedList> 
    inline constexpr bool isReducable_v = isReducable<Operation, Result, T, 
                                            T2ListToCheck, T2List, TestedList>::value;
}

template <typename T, typename Init, typename Element>
concept ReduceOperation = detail::isReducable_v<T, Init, Init, 
                            TypeList<Element>, TypeList<Element>, TypeList<>>; */

template <std::ranges::input_range Range, 
          std::indirectly_regular_unary_invocable<std::ranges::iterator_t<Range>> Proj = std::identity, 
          typename T = std::iter_value_t<std::projected<std::ranges::iterator_t<Range>, Proj>>, 
          typename BinaryOp = std::plus<>>
inline constexpr T reduce(Range&& range, Proj proj = {}, T init = {}, BinaryOp op = {}) {
    auto transformed = range | std::views::transform(proj);

    using Iter = std::ranges::iterator_t<decltype(transformed)>;
    using Sentinel = std::ranges::sentinel_t<decltype(transformed)>;

    if constexpr (std::same_as<Iter, Sentinel>) {
        return std::reduce(transformed.begin(), transformed.end(), init, op);
    } else {
        using CommonIter = std::common_iterator<Iter, Sentinel>;
        return std::reduce(CommonIter{transformed.begin()}, CommonIter{transformed.end()}, init, op);
    }
}

template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, 
        std::indirectly_regular_unary_invocable<Iter> Proj = std::identity, 
        typename T = std::iter_value_t<std::projected<Iter, Proj>>, 
        typename BinaryOp = std::plus<>>
inline constexpr T reduce(Iter first, Sentinel last, 
                          Proj proj = {}, T init = {}, BinaryOp op = {}) {
    return reduce(std::ranges::subrange(first, last), proj, init, op);
}

template <ExecutionPolicy Policy, std::ranges::input_range Range, 
        std::indirectly_regular_unary_invocable<std::ranges::iterator_t<Range>> Proj = std::identity, 
        typename T = std::iter_value_t<std::projected<std::ranges::iterator_t<Range>, Proj>>, 
        typename BinaryOp = std::plus<>>
inline T reduce(Policy&& policy, Range&& range, 
                Proj proj = {}, T init = {}, BinaryOp op = {}) {
    auto transformed = range | std::views::transform(proj);

    using Iter = std::ranges::iterator_t<decltype(transformed)>;
    using Sentinel = std::ranges::sentinel_t<decltype(transformed)>;

    if constexpr (std::same_as<Iter, Sentinel>) {
        return std::reduce(std::forward<Policy>(policy), transformed.begin(), transformed.end(), init, op);
    } else {
        using CommonIter = std::common_iterator<Iter, Sentinel>;
        return std::reduce(std::forward<Policy>(policy), 
            CommonIter{transformed.begin()}, CommonIter{transformed.end()}, init, op);
    }
}

template <ExecutionPolicy Policy, std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, 
        std::indirectly_regular_unary_invocable<Iter> Proj = std::identity, 
        typename T = std::iter_value_t<std::projected<Iter, Proj>>, 
        typename BinaryOp = std::plus<>>
inline T reduce(Policy&& policy, Iter first, Sentinel last, 
                Proj proj = {}, T init = {}, BinaryOp op = {}) {
    return reduce(std::forward<Policy>(policy), std::ranges::subrange(first, last), proj, init, op);
}

#endif