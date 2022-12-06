/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef LAND_TYPE
#define LAND_TYPE

#include <filesystem>
#include <format>
#include <string>
#include <concepts>
#include <bit>
#include <cstdint>

// bitmask
class LandType {
public:
    using Base = uint8_t;

    // don't use directly
    enum class Masks : Base {
        FEATURE   = 0b000000, // tile has feature, use like FEATURE | AIRDROME or AIRDROME
        PLAINS    = 0b000000,
        AIRDROME  = 0b000001,
        CRATER    = 0b000010,
        FIELD     = 0b000011,
        FLAG      = 0b000100,
        TREE      = 0b000101,
        BUSH      = 0b000110,
        HOUSE     = 0b000111,

        ROAD      = 0b100000, // tile has road, use like ROAD | NORTH | EAST
        ROAD_MASK = 0b001111,
        NORTH     = 0b001000,
        EAST      = 0b000100,
        SOUTH     = 0b000010,
        WEST      = 0b000001,

        MODIFIED  = 0b010000, // apply to get special meaning
        PLAINS2   = PLAINS | MODIFIED,
        TREES     = TREE   | MODIFIED,
        LOW_HOUSE = HOUSE  | MODIFIED,
        // also applyable to:
        // ROAD | NORTH | SOUTH 
        // ROAD | EAST  | WEST
    };
    using enum Masks;

    // WARNING: some of counted variants are invalid
    constexpr const static Base TOTAL_VARIANTS   = 64; // include with MODIFIED bit
    constexpr const static Base FEATURE_VARIANTS = 8;  // doesn't include with MODIFIED bit
    constexpr const static Base ROAD_VARIANTS    = 16; // doesn't include with MODIFIED bit

    constexpr LandType() = default;
    constexpr explicit LandType(Base type) : m_type{type} {}
    constexpr LandType(Masks type) : m_type{static_cast<Base>(type)} {}

    constexpr explicit operator bool() noexcept {
        return m_type != 0;
    }

    constexpr explicit operator Base() noexcept {
        return m_type;
    }

    constexpr LandType operator ~ () noexcept {
        return static_cast<LandType>(~m_type);
    }

    constexpr friend LandType operator | (LandType lhs, LandType rhs) noexcept {
        return static_cast<LandType>(lhs.m_type | rhs.m_type);
    }

    constexpr LandType& operator |= (LandType flags) noexcept {
        m_type |= flags.m_type;
        return *this;
    }

    constexpr friend LandType operator & (LandType lhs, LandType rhs) noexcept {
        return static_cast<LandType>(lhs.m_type & rhs.m_type);
    }

    constexpr LandType& operator &= (LandType flags) noexcept {
        m_type &= flags.m_type;
        return *this;
    }

    bool isModifiable() const noexcept;

    bool isValid() const noexcept;

    // number of roads leving the tile
    // WARNING: unsafe, use only if (*this) & ROAD
    int getRoadCount() const noexcept {
        return std::popcount(static_cast<Base>(*this & ROAD_MASK));
    }

    // only file name, add path to search by yourself
    std::filesystem::path getTextureFileName() const;
    std::string getName() const;

    template<std::invocable<LandType> Fn>
    static void forValid(Fn f);
private:
    Base m_type;

    constexpr explicit operator Masks() noexcept {
        return static_cast<Masks>(m_type);
    }
};

constexpr inline LandType operator ~ (LandType::Masks flags) noexcept {
    return ~static_cast<LandType>(flags);
}

constexpr inline LandType operator | (LandType::Masks lhs, LandType::Masks rhs) noexcept {
    return static_cast<LandType>(lhs) | static_cast<LandType>(rhs);
}

constexpr inline LandType operator & (LandType::Masks lhs, LandType::Masks rhs) noexcept {
    return static_cast<LandType>(lhs) & static_cast<LandType>(rhs);
}

template<std::invocable<LandType> Fn>
static void LandType::forValid(Fn f) {
    for (Base var = 0; var < FEATURE_VARIANTS; ++ var) {
        LandType type = FEATURE | static_cast<LandType>(var);
        if (type.isValid()) std::invoke(f, type);

        type |= MODIFIED;
        if (type.isValid()) std::invoke(f, type);
    }

    for (Base var = 0; var < ROAD_VARIANTS; ++ var) {
        LandType type = ROAD | static_cast<LandType>(var);
        if (type.isValid()) std::invoke(f, type);

        type |= MODIFIED;
        if (type.isValid()) std::invoke(f, type);
    }
}

// true if it's valid to place left tile next to the right tile (in horizontal row)
bool isCompatableHorizontal(LandType left, LandType right);

// true if it's valid to place top tile next to the down tile (in vertical row)
bool isCompatableVertical(LandType up, LandType down);

// true if roads form cycle
bool isCycle(LandType upLeft,   LandType upRight, 
             LandType downLeft, LandType downRight);

// will become a cycle after adding downRight
bool willBeCycle(LandType upLeft,   LandType upRight, 
                 LandType downLeft);

#endif