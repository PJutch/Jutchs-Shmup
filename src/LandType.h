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
#include <compare>
#include <bit>
#include <cstdint>

// bitmask
class LandType {
public:
    using Base = uint8_t;

    // don't use directly
    enum class Masks : Base {
        MODIFIED  = 0b0010000, // apply to get special meaning

        FEATURE   = 0b0000000, // tile has feature, use like FEATURE | AIRDROME or AIRDROME
        PLAINS    = 0b0000000,
        AIRDROME  = 0b0000001,
        CRATER    = 0b0000010,
        FIELD     = 0b0000011,
        FLAG      = 0b0000100,
        TREE      = 0b0000101,
        BUSH      = 0b0000110,
        HOUSE     = 0b0000111,

        PLAINS2   = PLAINS | MODIFIED,
        TREES     = TREE   | MODIFIED,
        LOW_HOUSE = HOUSE  | MODIFIED,

        ROAD      = 0b0100000, // tile has road, use like ROAD | NORTH | EAST
        // MODIFIED applyable to:
        // ROAD | NORTH | SOUTH 
        // ROAD | EAST  | WEST

        WATER     = 0b1000000,
        // can be used by itself or combined with NORTH, EAST, SOUTH, WEST to make shores
        ISLANDS  = WATER | MODIFIED,
        // MODIFIED also applyable to:
        // WATER | NORTH | EAST 
        // WATER | NORTH | WEST 
        // WATER | SOUTH | EAST 
        // WATER | SOUTH | WEST 
        // to make water corners

        NORTH     = 0b0001000,
        EAST      = 0b0000100,
        SOUTH     = 0b0000010,
        WEST      = 0b0000001,
        DIR_MASK  = 0b0001111, // mask for internal use
    };
    using enum Masks;

    // WARNING: some of counted variants are invalid
    constexpr const static Base TOTAL_VARIANTS   = 128; // include with MODIFIED bit
    constexpr const static Base FEATURE_VARIANTS = 8;  // doesn't include with MODIFIED bit
    constexpr const static Base ROAD_VARIANTS    = 16; // doesn't include with MODIFIED bit
    constexpr const static Base WATER_VARIANTS   = 16; // doesn't include with MODIFIED bit

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

    constexpr friend auto operator <=> (LandType lhs, LandType rhs) noexcept = default;
    bool isModifiable() const noexcept;

    bool isValid() const noexcept;

    // number of roads leaving the tile / number of shores
    // WARNING: unsafe, use only if (*this & ROAD) || (*this & WATER)
    int getActiveDirCount() const noexcept {
        return std::popcount(static_cast<Base>(*this & DIR_MASK));
    }

    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    bool hasRoadSide(LandType side) const noexcept {
        return (*this & ROAD) && (*this & side);
    }

    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    bool hasWaterSide(LandType side) const noexcept {
        return (*this & WATER) && (!(*this & DIR_MASK) 
                                || !(*this & MODIFIED) && (*this & side));
    }

    // WARNING: corner must be a combination of two dir flags that form a corner
    // e. g. sit must be one of NORTH | EAST, NORTH | WEST, SOUTH | EAST, SOUTH | WEST
    bool hasWaterCorner(LandType corner) const noexcept {
        return (*this & WATER) && ( !(*this & DIR_MASK) 
                                 ||  (*this & corner) && !(*this & MODIFIED) 
                                 ||  (*this & DIR_MASK) == corner);
    }

    // true if has water on this side or to adjenct corners
    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    bool hasWaterAny(LandType side) const noexcept;

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
        if (type.isModifiable()) std::invoke(f, type | MODIFIED);
    }

    for (Base var = 0; var < ROAD_VARIANTS; ++ var) {
        LandType type = ROAD | static_cast<LandType>(var);

        if (type.isValid()) std::invoke(f, type);
        if (type.isModifiable()) std::invoke(f, type | MODIFIED);
    }

    for (Base var = 0; var < WATER_VARIANTS; ++ var) {
        LandType type = WATER | static_cast<LandType>(var);

        if (type.isValid()) std::invoke(f, type);
        if (type.isModifiable()) std::invoke(f, type | MODIFIED);
    }
}

// true if it's valid to place the left tile next to the right tile (in horizontal row)
bool isCompatableHorizontal(LandType left, LandType right);

// true if it's valid to place the up tile next to the down tile (in vertical row)
bool isCompatableVertical(LandType up, LandType down);

// true if it's valid to place the upLeft tile next to the downRight tile (in diagonal manner)
bool isCompatableDiagonal(LandType upLeft, LandType downRight);

// true if it's valid to place the downLeft tile next to the upRight tile (in anti-diagonal manner)
bool isCompatableAntiDiagonal(LandType downLeft, LandType upRight);

// true if roads form cycle
bool isCycle(LandType upLeft,   LandType upRight, 
             LandType downLeft, LandType downRight);

// will become a cycle after adding downRight
bool willBeCycle(LandType upLeft,   LandType upRight, 
                 LandType downLeft);

#endif