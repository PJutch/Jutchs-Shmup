/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_FLAGS_
#define AIRPLANE_FLAGS_

#include <cstdint>
#include <type_traits>
#include <filesystem>

namespace Airplane {
    // bitmask
    enum class Flags : uint8_t {
        ENEMY_SIDE  = 0b0000000,
        PLAYER_SIDE = 0b0000001,

        LIGHT       = 0b0000000,
        HEAVY       = 0b0000010,
        SLOW        = 0b0000000,
        FAST        = 0b0000100,
        NO_WEAPON   = 0b0000000,
        HAS_WEAPON  = 0b0001000,

        NO_PICKUPS  = 0b0000000,
        USE_PICKUPS = 0b0100000,
        NO_BOMB     = 0b0000000,
        HAS_BOMB    = 0b0010000,

        TEXTURE     = 0b0011111, // flags used to determinate texture, must be continuous
        RUNTIME     = 0b0100001, // flags used to tweak runtime behaviour
    };

    const int TEXTURE_VARIANTS = static_cast<int>(Flags::TEXTURE) + 1;

    inline constexpr Flags operator ~ (Flags flags) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(~static_cast<Base>(flags));
    }

    inline constexpr Flags operator | (Flags lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(static_cast<Base>(lhs) | static_cast<Base>(rhs));
    }

    inline constexpr Flags& operator |= (Flags& lhs, Flags rhs) noexcept {
        lhs = lhs | rhs;
        return lhs;
    }

    inline constexpr Flags operator & (Flags lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(static_cast<Base>(lhs) & static_cast<Base>(rhs));
    }

    inline constexpr Flags& operator &= (Flags& lhs, Flags rhs) noexcept {
        lhs = lhs & rhs;
        return lhs;
    }

    inline constexpr bool test(Flags lhs, Flags rhs) noexcept {
        return static_cast<bool>(lhs & rhs);
    }

    std::filesystem::path getTextureFileName(Flags flags);
    std::string getTextureName(Flags flags);
}

#endif