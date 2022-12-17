/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef LAND_TYPE_H_
#define LAND_TYPE_H_

#include <filesystem>
#include <format>
#include <string>
#include <concepts>
#include <compare>
#include <bit>
#include <cstdint>

namespace Land {
    // bitmask
    enum class Type : uint8_t {
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
    constexpr const static size_t TOTAL_VARIANTS = 256; // with MODIFIED or BADLAND tiles
    constexpr const static size_t FEATURE_VARIANTS = 9;  // withot MODIFIED or BADLAND tiles
    constexpr const static size_t ROAD_VARIANTS    = 16; // withot MODIFIED or BADLAND tiles
    constexpr const static size_t WATER_VARIANTS   = 16; // withot MODIFIED or BADLAND tiles

    inline constexpr Type operator ~ (Type flags) noexcept {
        using Base = std::underlying_type_t<Type>;
        return static_cast<Type>(~static_cast<Base>(flags));
    }

    inline constexpr Type operator | (Type lhs, Type rhs) noexcept {
        using Base = std::underlying_type_t<Type>;
        return static_cast<Type>(static_cast<Base>(lhs) | static_cast<Base>(rhs));
    }

    inline constexpr Type& operator |= (Type& lhs, Type rhs) noexcept {
        lhs = lhs | rhs;
        return lhs;
    }

    inline constexpr Type operator & (Type lhs, Type rhs) noexcept {
        using Base = std::underlying_type_t<Type>;
        return static_cast<Type>(static_cast<Base>(lhs) & static_cast<Base>(rhs));
    }

    inline constexpr Type& operator &= (Type& lhs, Type rhs) noexcept {
        lhs = lhs & rhs;
        return lhs;
    }

    inline constexpr bool test(Type lhs, Type rhs) noexcept {
        return static_cast<bool>(lhs & rhs);
    }

    bool isModifiable(Type type) noexcept;

    inline bool hasBadlandVariant(Type type) noexcept {
        return type != (Type::FEATURE | Type::BUSH) 
            && type != Type::WATER;
    }

    bool isValid(Type type) noexcept;

    // number of roads leaving the tile / number of shores
    // WARNING: unsafe, use only if (*this & ROAD) || (*this & WATER)
    inline int getActiveDirCount(Type type) noexcept {
        using Base = std::underlying_type_t<Type>;
        return std::popcount(static_cast<Base>(type & Type::DIR_MASK));
    }

    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    inline bool hasRoadSide(Type type, Type side) noexcept {
        return test(type, Type::ROAD) && test(type, side);
    }

    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    inline bool hasWaterSide(Type type, Type side) noexcept {
        return test(type, Type::WATER) && (!test(type, Type::DIR_MASK) 
                                        || !test(type, Type::MODIFIED) && test(type, side));
    }

    // WARNING: corner must be a combination of two dir flags that form a corner
    // e. g. sit must be one of NORTH | EAST, NORTH | WEST, SOUTH | EAST, SOUTH | WEST
    inline bool hasWaterCorner(Type type, Type corner) noexcept {
        return test(type, Type::WATER) && ( !test(type,  Type::DIR_MASK) 
                                            ||  test(type, corner) && !test(type,  Type::MODIFIED) 
                                            ||  (type & Type::DIR_MASK) == corner);
    }

    // true if has water on this side or to adjenct corners
    // WARNING: side must be a single dir flag
    // e. g. it must be one of NORTH, EAST, SOUTH, WEST
    bool hasWaterAny(Type type, Type side) noexcept;

    // only file name, add path to search by yourself
    std::filesystem::path getTextureFileName(Type type);
    std::string getName(Type type);

    template<std::invocable<Type> Fn>
    inline void forValidVariants(Type type, Fn&& f) {
        if (isValid(type)) {
            std::invoke(std::forward<Fn>(f), type);
            if (isModifiable(type)) {
                std::invoke(std::forward<Fn>(f), type | Type::MODIFIED);
                if (hasBadlandVariant(type | Type::MODIFIED))
                    std::invoke(std::forward<Fn>(f), type | Type::MODIFIED | Type::BADLAND);
            }

            if (hasBadlandVariant(type))
                std::invoke(std::forward<Fn>(f), type | Type::BADLAND);
        }
    }

    template<std::invocable<Type> Fn>
    void forValidFeature(Fn&& f) {
        for (std::underlying_type_t<Type> var = 0; var < FEATURE_VARIANTS; ++ var) {
            Type type = Type::FEATURE | static_cast<Type>(var);
            forValidVariants(type, std::forward<Fn>(f));
        }
    }

    template<std::invocable<Type> Fn>
    void forValidRoad(Fn&& f) {
        for (std::underlying_type_t<Type> var = 0; var < ROAD_VARIANTS; ++ var) {
            Type type = Type::ROAD | static_cast<Type>(var);
            forValidVariants(type, std::forward<Fn>(f));
        }
    }

    template<std::invocable<Type> Fn>
    void forValidWater(Fn&& f) {
        for (std::underlying_type_t<Type> var = 0; var < WATER_VARIANTS; ++ var) {
            Type type = Type::WATER | static_cast<Type>(var);
            forValidVariants(type, std::forward<Fn>(f));
        }
    }

    template<std::invocable<Type> Fn>
    void forValid(Fn&& f) {
        forValidFeature(std::forward<Fn>(f));
        forValidRoad(std::forward<Fn>(f));
        forValidWater(std::forward<Fn>(f));
    }

    // true if it's valid to place the left tile next to the right tile (in horizontal row)
    bool isCompatableHorizontal(Type left, Type right);

    // true if it's valid to place the up tile next to the down tile (in vertical row)
    bool isCompatableVertical(Type up, Type down);

    // true if it's valid to place the upLeft tile next to the downRight tile (in diagonal manner)
    bool isCompatableDiagonal(Type upLeft, Type downRight);

    // true if it's valid to place the downLeft tile next to the upRight tile (in anti-diagonal manner)
    bool isCompatableAntiDiagonal(Type downLeft, Type upRight);

    inline Type destroyed(Type type) noexcept {
        if (test(type, Type::WATER) || test(type, Type::ROAD))
            return type;
        return Type::CRATER | type & Type::BADLAND;
    }

    int scoreIfDestroyed(Type type);

    inline bool isEnemyTarget(Type type) {
        return (type & ~Type::MODIFIED & ~Type::BADLAND) == Type::PLAYER_FLAG;
    }
}

#endif