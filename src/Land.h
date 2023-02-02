/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef LAND_H_
#define LAND_H_

#include <filesystem>
#include <format>
#include <string>
#include <concepts>
#include <compare>
#include <bit>
#include <cstdint>

// bitmask
enum class Land : uint8_t {
    MODIFIED  = 0b00010000, // apply to get special meaning

    GRASS     = 0b00000000,
    BADLAND   = 0b10000000,

    FEATURE     = 0b00000000, // tile has feature, use like FEATURE | AIRDROME or AIRDROME
    PLAINS      = 0b00000000,
    AIRDROME    = 0b00000001,
    CRATER      = 0b00000010,
    FIELD       = 0b00000011,
    PLAYER_FLAG = 0b00000100,
    ENEMY_FLAG  = 0b00001000,
    TREE        = 0b00000101,
    BUSH        = 0b00000110,
    HOUSE       = 0b00000111,

    PLAINS2   = PLAINS | MODIFIED,
    TREES     = TREE   | MODIFIED,
    LOW_HOUSE = HOUSE  | MODIFIED,

    ROAD      = 0b00100000, // tile has road, use like ROAD | NORTH | EAST
    // MODIFIED applyable to:
    // ROAD | NORTH | SOUTH 
    // ROAD | EAST  | WEST

    WATER     = 0b01000000,
    // can be used by itself or combined with NORTH, EAST, SOUTH, WEST to make shores
    ISLANDS  = WATER | MODIFIED,
    // MODIFIED also applyable to:
    // WATER | NORTH | EAST 
    // WATER | NORTH | WEST 
    // WATER | SOUTH | EAST 
    // WATER | SOUTH | WEST 
    // to make water corners

    NORTH     = 0b00001000,
    EAST      = 0b00000100,
    SOUTH     = 0b00000010,
    WEST      = 0b00000001,
    DIR_MASK  = 0b00001111, // mask for internal use
};

// WARNING: some of counted variants are invalid
constexpr const static size_t LAND_VARIANTS = 256; // with MODIFIED or BADLAND tiles
constexpr const static size_t LAND_FEATURE_VARIANTS = 9;  // withot MODIFIED or BADLAND tiles
constexpr const static size_t LAND_ROAD_VARIANTS    = 16; // withot MODIFIED or BADLAND tiles
constexpr const static size_t LAND_WATER_VARIANTS   = 16; // withot MODIFIED or BADLAND tiles

inline constexpr Land operator ~ (Land flags) noexcept {
    using Base = std::underlying_type_t<Land>;
    return static_cast<Land>(~static_cast<Base>(flags));
}

inline constexpr Land operator | (Land lhs, Land rhs) noexcept {
    using Base = std::underlying_type_t<Land>;
    return static_cast<Land>(static_cast<Base>(lhs) | static_cast<Base>(rhs));
}

inline constexpr Land& operator |= (Land& lhs, Land rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

inline constexpr Land operator & (Land lhs, Land rhs) noexcept {
    using Base = std::underlying_type_t<Land>;
    return static_cast<Land>(static_cast<Base>(lhs) & static_cast<Base>(rhs));
}

inline constexpr Land& operator &= (Land& lhs, Land rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

inline constexpr bool test(Land lhs, Land rhs) noexcept {
    return static_cast<bool>(lhs & rhs);
}

bool isModifiable(Land land) noexcept;

inline bool hasBadlandVariant(Land land) noexcept {
    return land != (Land::FEATURE | Land::BUSH) 
        && land != Land::WATER;
}

bool isValid(Land land) noexcept;

// number of roads leaving the tile / number of shores
// WARNING: unsafe, use only if (*this & ROAD) || (*this & WATER)
inline int getActiveDirCount(Land land) noexcept {
    using Base = std::underlying_type_t<Land>;
    return std::popcount(static_cast<Base>(land & Land::DIR_MASK));
}

// WARNING: side must be a single dir flag
// e. g. it must be one of NORTH, EAST, SOUTH, WEST
inline bool hasRoadSide(Land land, Land side) noexcept {
    return test(land, Land::ROAD) && test(land, side);
}

// WARNING: side must be a single dir flag
// e. g. it must be one of NORTH, EAST, SOUTH, WEST
inline bool hasWaterSide(Land land, Land side) noexcept {
    return test(land, Land::WATER) && (!test(land, Land::DIR_MASK) 
                                    || !test(land, Land::MODIFIED) && test(land, side));
}

// WARNING: corner must be a combination of two dir flags that form a corner
// e. g. sit must be one of NORTH | EAST, NORTH | WEST, SOUTH | EAST, SOUTH | WEST
inline bool hasWaterCorner(Land land, Land corner) noexcept {
    return test(land, Land::WATER) && (  !test(land,  Land::DIR_MASK) 
                                       || test(land, corner) && !test(land,  Land::MODIFIED) 
                                       || (land & Land::DIR_MASK) == corner);
}

// true if has water on this side or to adjenct corners
// WARNING: side must be a single dir flag
// e. g. it must be one of NORTH, EAST, SOUTH, WEST
bool hasWaterAny(Land land, Land side) noexcept;

// only file name, add path to search by yourself
std::filesystem::path getTextureFileName(Land land);
std::string getName(Land land);

template<std::invocable<Land> Fn>
inline void forValidVariants(Land land, Fn&& f) {
    if (isValid(land)) {
        std::invoke(std::forward<Fn>(f), land);
        if (isModifiable(land)) {
            std::invoke(std::forward<Fn>(f), land | Land::MODIFIED);
            if (hasBadlandVariant(land | Land::MODIFIED))
                std::invoke(std::forward<Fn>(f), land | Land::MODIFIED | Land::BADLAND);
        }

        if (hasBadlandVariant(land))
            std::invoke(std::forward<Fn>(f), land | Land::BADLAND);
    }
}

template<std::invocable<Land> Fn>
void forValidFeature(Fn&& f) {
    for (std::underlying_type_t<Land> var = 0; var < LAND_FEATURE_VARIANTS; ++ var) {
        Land land = Land::FEATURE | static_cast<Land>(var);
        forValidVariants(land, std::forward<Fn>(f));
    }
}

template<std::invocable<Land> Fn>
void forValidRoad(Fn&& f) {
    for (std::underlying_type_t<Land> var = 0; var < LAND_ROAD_VARIANTS; ++ var) {
        Land land = Land::ROAD | static_cast<Land>(var);
        forValidVariants(land, std::forward<Fn>(f));
    }
}

template<std::invocable<Land> Fn>
void forValidWater(Fn&& f) {
    for (std::underlying_type_t<Land> var = 0; var < LAND_WATER_VARIANTS; ++ var) {
        Land land = Land::WATER | static_cast<Land>(var);
        forValidVariants(land, std::forward<Fn>(f));
    }
}

template<std::invocable<Land> Fn>
void forValidLand(Fn&& f) {
    forValidFeature(std::forward<Fn>(f));
    forValidRoad(std::forward<Fn>(f));
    forValidWater(std::forward<Fn>(f));
}

// true if it's valid to place the left tile next to the right tile (in horizontal row)
bool isCompatableHorizontal(Land left, Land right);

// true if it's valid to place the up tile next to the down tile (in vertical row)
bool isCompatableVertical(Land up, Land down);

// true if it's valid to place the upLeft tile next to the downRight tile (in diagonal manner)
bool isCompatableDiagonal(Land upLeft, Land downRight);

// true if it's valid to place the downLeft tile next to the upRight tile (in anti-diagonal manner)
bool isCompatableAntiDiagonal(Land downLeft, Land upRight);

inline Land destroyed(Land land) noexcept {
    if (test(land, Land::WATER) || test(land, Land::ROAD))
        return land;
    return Land::CRATER | land & Land::BADLAND;
}

int scoreIfDestroyed(Land land);

inline bool isEnemyTarget(Land land) noexcept {
    return (land & ~Land::MODIFIED & ~Land::BADLAND) == Land::PLAYER_FLAG;
}

inline bool canHaveTurret(Land land) noexcept {
    return (land & ~Land::MODIFIED & ~Land::BADLAND) == Land::PLAINS;
}

#endif