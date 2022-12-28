/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef CHANCE_TABLE_H_
#define CHANCE_TABLE_H_

#include "algorithm.h"
#include "RangeAdaptor.h"

#include <random>
#include <stdexcept>
#include <ranges>
#include <iterator>
#include <concepts>
#include <type_traits>

namespace ChanceTable {
    template <typename T>
    struct BasicEntry {
        using value_type = T;

        value_type value;
        double chance;
    };

    template <typename T>
    auto& value(BasicEntry<T>& entry) noexcept {
        return entry.value;
    }

    template <typename T>
    const auto& value(const BasicEntry<T>& entry) noexcept {
        return entry.value;
    }

    template <typename T>
    inline double& chance(BasicEntry<T>& entry) noexcept {
        return entry.chance;
    }

    template <typename T>
    inline double chance(const BasicEntry<T>& entry) noexcept {
        return entry.chance;
    }

    template <typename T>
    concept ConstEntry = requires (const T ct) {
        typename T::value_type;
        { value(ct) } -> std::same_as<const typename T::value_type&>;
        { chance(ct) } -> std::convertible_to<double>;
    };

    template <typename T>
    concept Entry = ConstEntry<T> && requires (T t, const T ct) {
        typename T::value_type;
        { value(t) } -> std::same_as<typename T::value_type&>;
        { chance(t) } -> std::convertible_to<double&>;
    };

    template <typename T>
    struct isEntryAdaptor {
        inline static const constexpr bool value = false;
    };

    template <typename T>
    inline constexpr bool isEntryAdaptor_v = isEntryAdaptor<std::remove_cvref_t<T>>::value;

    template <typename T>
    concept EntryAdaptor = isEntryAdaptor_v<T>;

    template <ConstEntry Base, EntryAdaptor Adaptor>
    decltype(auto) operator | (const Base& entry, Adaptor&& adaptor) noexcept {
        return std::forward<Adaptor>(adaptor)(entry);
    }

    namespace detail {
        template <EntryAdaptor Adaptor1, EntryAdaptor Adaptor2>
        class CombinedAdaptor {
        public:
            CombinedAdaptor(Adaptor1 adaptor1, Adaptor2 adaptor2) noexcept : 
                m_adaptor1{std::move(adaptor1)}, m_adaptor2{std::move(adaptor2)} {}

            template <ConstEntry Base>
            decltype(auto) operator () (const Base& entry) noexcept {
                return m_fn2(m_fn1(entry));
            }
        private:
            Adaptor1 m_adaptor1;
            Adaptor2 m_adaptor2;
        };
    }

    template <EntryAdaptor Adaptor1, EntryAdaptor Adaptor2>
    struct isEntryAdaptor<detail::CombinedAdaptor<Adaptor1, Adaptor2>> {
        inline static const constexpr bool value = true;
    };

    template <EntryAdaptor Adaptor1, EntryAdaptor Adaptor2>
    inline detail::CombinedAdaptor<Adaptor1, Adaptor2> operator | 
            (Adaptor1&& adaptor1, Adaptor2&& adaptor2) noexcept {
        return {std::forward<Adaptor1>(adaptor1), std::forward<Adaptor2>(adaptor2)};
    }

    namespace views {
        namespace detail {
            template <ConstEntry Base, std::regular_invocable<double> Proj = std::identity>
                requires std::convertible_to<std::invoke_result_t<Proj, double>, double>
            class TransformChanceView  {
            public:
                using value_type = Base::value_type;

                TransformChanceView(const Base& base, Proj proj) noexcept :
                    m_base{base}, m_proj{std::move(proj)} {}

                friend const value_type& value(const TransformChanceView<Base, Proj>& entry) noexcept {
                    return value(entry.m_base);
                }
                
                friend double chance(const TransformChanceView<Base, Proj>& entry) {
                    return entry.m_proj(chance(entry.m_base));
                }   
            private:
                const Base& m_base;
                Proj m_proj;
            };
            template <std::regular_invocable<double> Proj = std::identity>
                requires std::convertible_to<std::invoke_result_t<Proj, double>, double>
            class TransformChanceAdaptor {
            public:
                explicit TransformChanceAdaptor(Proj proj) noexcept : m_proj{std::move(proj)} {}

                template <ConstEntry Base>
                auto operator () (const Base& entry) const noexcept {
                    return detail::TransformChanceView<Base, Proj>{entry, m_proj};
                }
            private:
                Proj m_proj;
            };
        }
    }

    template <std::regular_invocable<double> Proj>
    struct isEntryAdaptor<views::detail::TransformChanceAdaptor<Proj>> {
        inline static const constexpr bool value = true;
    };

    namespace views {
        template <std::regular_invocable<double> Proj = std::identity>
                requires std::convertible_to<std::invoke_result_t<Proj, double>, double>
        auto transformChance(Proj&& proj) noexcept {
            return detail::TransformChanceAdaptor{std::forward<Proj>(proj)};
        }

        template <ConstEntry Base, std::regular_invocable<double> Proj = std::identity>
                requires std::convertible_to<std::invoke_result_t<Proj, double>, double>
        auto transformChance(const Base& entry, Proj&& proj) noexcept {
            return entry | detail::TransformChanceAdaptor{std::forward<Proj>(proj)};
        }
    }
    
    class Invalid : public std::logic_error {
        using std::logic_error::logic_error;
    };

    template <std::forward_iterator Iter, std::sentinel_for<Iter> Sentinel, 
            std::uniform_random_bit_generator Engine> 
        requires ConstEntry<std::iter_value_t<Iter>>
    inline decltype(auto) getRandom(Iter first, Sentinel last, Engine& engine) {
        return getRandom(std::ranges::subrange(first, last), engine);
    }

    template <std::ranges::forward_range Range, std::uniform_random_bit_generator Engine> 
        requires ConstEntry<std::ranges::range_value_t<Range>>
    decltype(auto) getRandom(Range&& range, Engine& engine) {
        double seed = std::uniform_real_distribution{0.0, 1.0}(engine);
        for (const auto& entry : range) {
            if (seed < chance(entry)) {
                return value(entry);
            } else {
                seed -= chance(entry);
            }
        }

        throw Invalid("Sum of chances in the table must be >= 1.0");
    }

    template <std::forward_iterator Iter, std::sentinel_for<Iter> Sentinel> 
        requires Entry<std::iter_value_t<Iter>>
    inline void normalize(Iter first, Sentinel last) {
        normalize(std::ranges::subrange(first, last));
    }

    template <std::ranges::forward_range Range> 
        requires Entry<std::ranges::range_value_t<Range>>
    void normalize(Range& range) {
        double sumChance = reduce(range, [](const auto& entry) {
            return chance(entry);
        });

        for (auto& entry : range)
            entry.chance /= sumChance;
    }

    namespace views {
        namespace detail {
            struct NormalizeRangeAdaptor {
                template <std::ranges::range Range> 
                    requires ConstEntry<std::ranges::range_value_t<Range>>
                decltype(auto) operator () (Range&& range) const noexcept {
                    double sumChance = reduce(std::forward<Range>(range), [](const auto& entry) {
                        return chance(entry);
                    });

                    return range
                        | std::views::transform([sumChance](const auto& entry) {
                            return entry 
                                | views::transformChance([sumChance](double chance) -> double {
                                    return chance / sumChance;
                                });
                        });
                }
            };

            using ::operator | ; // to use operator | for RangeAdaptor
        } 

        inline constexpr detail::NormalizeRangeAdaptor normalize;
    }
}

template <>
struct isRangeAdaptor<ChanceTable::views::detail::NormalizeRangeAdaptor> {
    inline static const constexpr bool value = true;
};

#endif
