/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "RangeAdaptor.h"

#include <numeric>
#include <execution>
#include <ranges>
#include <iterator>
#include <concepts>
#include <type_traits>

template <typename T>
concept ExecutionPolicy = std::is_execution_policy_v<std::remove_cvref_t<T>>;

template <typename T>
using asIter_t = std::conditional_t<std::ranges::range<T>, 
                                    std::ranges::iterator_t<T>, 
                                    T>;

template <typename Proj, typename T>
concept ProjectionFor = std::indirectly_regular_unary_invocable<Proj, asIter_t<T>>;

template <typename T, ProjectionFor<T> Proj>
using projected_t = std::projected<asIter_t<T>, Proj>;

template <typename T, ProjectionFor<T> Proj>
using projectedValue_t = std::iter_value_t<projected_t<T, Proj>>;

template <typename Comp, typename T, typename Proj>
concept ComparatorFor = std::indirect_strict_weak_order<Comp, projected_t<T, Proj>>;

template <std::ranges::range Range>
auto to_common_range(Range&& range) noexcept {
    return std::forward<Range>(range) | std::views::common;
}

namespace views {
    namespace detail {
        class CommonAdaptor {
        public:
            template <std::ranges::range Range>
            decltype(auto) operator () (Range&& range) const noexcept {
                if constexpr (std::ranges::common_range<Range>) {
                    return std::forward<Range>(range) | std::views::common;
                } else {
                    return std::forward<Range>(range);
                }
            }
        };
    }

    inline const constexpr detail::CommonAdaptor common;
}

template <>
struct isRangeAdaptor<views::detail::CommonAdaptor> {
    inline static const constexpr bool value = true;
};

template <std::ranges::input_range Range, 
    ProjectionFor<Range> Proj = std::identity, 
    typename T = projectedValue_t<Range, Proj>, 
    typename BinaryOp = std::plus<>>
inline constexpr T reduce(Range&& range, Proj proj = {}, T init = {}, BinaryOp op = {}) {
    auto transformed = range | std::views::transform(proj) | views::common;
    return std::reduce(transformed.begin(), transformed.end(), std::move(init), std::move(op));
}

template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, 
    ProjectionFor<Iter> Proj = std::identity, 
    typename T = projectedValue_t<Iter, Proj>, 
    typename BinaryOp = std::plus<>>
inline constexpr T reduce(Iter first, Sentinel last, 
                          Proj proj = {}, T init = {}, BinaryOp op = {}) {
    return reduce(std::ranges::subrange(first, last), 
                  std::move(proj), std::move(init), std::move(op));
}

template <ExecutionPolicy Policy, std::ranges::input_range Range, 
    ProjectionFor<Range> Proj = std::identity, 
    typename T = projectedValue_t<Range, Proj>, 
    typename BinaryOp = std::plus<>>
inline T reduce(Policy&& policy, Range&& range, 
                Proj proj = {}, T init = {}, BinaryOp op = {}) {
    auto transformed = range | std::views::transform(proj) | views::common;
    return std::reduce(std::forward<Policy>(policy), 
        transformed.begin(), transformed.end(), std::move(init), std::move(op));
}

template <ExecutionPolicy Policy, std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, 
    ProjectionFor<Iter> Proj = std::identity, 
    typename T = projectedValue_t<Iter, Proj>, 
    typename BinaryOp = std::plus<>>
inline T reduce(Policy&& policy, Iter first, Sentinel last, 
                Proj proj = {}, T init = {}, BinaryOp op = {}) {
    return reduce(std::forward<Policy>(policy), std::ranges::subrange(first, last), proj, init, op);
}

template<std::ranges::forward_range Range, 
    ProjectionFor<Range> Proj = std::identity, 
    typename T = projectedValue_t<Range, Proj>, 
    ComparatorFor<Range, Proj> Comp = std::ranges::less>
constexpr T min_value(Range&& range, T default_value = {}, Comp comp = {}, Proj proj = {}) {
    auto smallest = std::ranges::min_element(range, std::move(comp), std::move(proj));
    return (smallest == range.end()) ? default_value : std::invoke(proj, *smallest);
}

template<std::forward_iterator Iter, std::sentinel_for<Iter> Sentinel, 
    ProjectionFor<Iter> Proj = std::identity,
    typename T = projectedValue_t<Iter, Proj>, 
    ComparatorFor<Iter, Proj> Comp = std::ranges::less>
constexpr T min_value(Iter first, Sentinel last, T default_value = {}, 
                              Comp comp = {}, Proj proj = {}) {
    return min_value(std::ranges::subrange(first, last), 
        std::move(default_value), std::move(comp), std::move(proj));
}

template<std::ranges::forward_range Range, 
    ProjectionFor<Range> Proj = std::identity,
    typename T = projectedValue_t<Range, Proj>, 
    ComparatorFor<Range, Proj> Comp = std::ranges::less>
constexpr T max_value(Range&& range, T default_value = {}, Comp comp = {}, Proj proj = {}) {
    auto largest = std::ranges::max_element(range, comp, proj);
    return (largest == range.end()) ? default_value : std::invoke(proj, *largest);
}

template<std::forward_iterator Iter, std::sentinel_for<Iter> Sentinel, 
    ProjectionFor<Iter> Proj = std::identity,
    typename T = projectedValue_t<Iter, Proj>, 
    ComparatorFor<Iter, Proj> Comp = std::ranges::less>
constexpr T max_value(Iter first, Sentinel last, T default_value = {}, 
                              Comp comp = {}, Proj proj = {}) {
    return max_value(std::ranges::subrange(first, last), 
        std::move(default_value), std::move(comp), std::move(proj));
}

#endif