/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef RANGE_ADAPTOR_H_
#define RANGE_ADAPTOR_H_

#include <ranges>
#include <concepts>
#include <type_traits>

template <typename T>
struct isRangeAdaptor {
    inline static const constexpr bool value = false;
};

template <typename T>
inline constexpr bool isRangeAdaptor_v = isRangeAdaptor<std::remove_cvref_t<T>>::value;

template <typename T>
concept RangeAdaptor = isRangeAdaptor_v<T>;

template <std::ranges::range Range, RangeAdaptor Adaptor> 
decltype(auto) operator | (Range&& range, Adaptor&& adaptor) noexcept {
    return std::forward<Adaptor>(adaptor)(std::forward<Range>(range));
}

namespace detail {
    template <typename Adaptor1, typename Adaptor2>
    class CombinedAdaptor {
    public:
        CombinedAdaptor(      Adaptor1&& adaptor1,       Adaptor2&& adaptor2) noexcept : 
            m_adaptor1{std::move(adaptor1)}, m_adaptor2{std::move(adaptor2)} {}
        CombinedAdaptor(      Adaptor1&& adaptor1, const Adaptor2&  adaptor2) noexcept : 
            m_adaptor1{std::move(adaptor1)}, m_adaptor2{          adaptor2 } {}
        CombinedAdaptor(const Adaptor1&  adaptor1,       Adaptor2&& adaptor2) noexcept : 
            m_adaptor1{          adaptor1 }, m_adaptor2{std::move(adaptor2)} {}
        CombinedAdaptor(const Adaptor1&  adaptor1, const Adaptor2&  adaptor2) noexcept : 
            m_adaptor1{          adaptor1 }, m_adaptor2{          adaptor2 } {}

        template <std::ranges::range Range>
        decltype(auto) operator () (Range&& range) noexcept {
            return m_fn2(m_fn1(std::forward<Range>(range)));
        }
    private:
        Adaptor1 m_adaptor1;
        Adaptor2 m_adaptor2;
    };
}

template <typename Adaptor1, typename Adaptor2>
struct isRangeAdaptor<detail::CombinedAdaptor<Adaptor1, Adaptor2>> {
    inline static const constexpr bool value = true;
};

template <typename Adaptor1, typename Adaptor2> 
    requires RangeAdaptor<Adaptor1> || RangeAdaptor<Adaptor2>
inline detail::CombinedAdaptor<Adaptor1, Adaptor2> operator | 
        (Adaptor1&& adaptor1, Adaptor1&& adaptor2) noexcept {
    return {std::forward<Adaptor1>(adaptor1), std::forward<Adaptor2>(adaptor2)};
}

#endif