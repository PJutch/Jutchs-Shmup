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

namespace Airplane {
    // bitmask
    enum class Flags : uint8_t {
        ENEMY_SIDE  = 0b000000,
        PLAYER_SIDE = 0b000001,

        LIGHT       = 0b000000,
        HEAVY       = 0b000010,
        SLOW        = 0b000000,
        FAST        = 0b000100,
        NO_WEAPON   = 0b000000,
        HAS_WEAPON  = 0b001000,

        UNIQUE      = 0b000000,
        DELETABLE   = 0b010000,
        NO_PICKUPS  = 0b000000,
        USE_PICKUPS = 0b100000,

        TEXTURE     = 0b001111, // flags used to determinate texture, must be continuous
        RUNTIME     = 0b110001, // flags used to tweak runtime behaviour
    };

    const int TEXTURE_VARIANTS = static_cast<int>(Flags::TEXTURE) + 1;

    inline Flags operator ~ (Flags flags) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(~static_cast<Base>(flags));
    }

    inline Flags operator | (Flags lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(static_cast<Base>(lhs) | static_cast<Base>(rhs));
    }

    inline Flags& operator |= (Flags& lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        lhs = lhs | rhs;
        return lhs;
    }

    inline Flags operator & (Flags lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        return static_cast<Flags>(static_cast<Base>(lhs) & static_cast<Base>(rhs));
    }

    inline Flags& operator &= (Flags& lhs, Flags rhs) noexcept {
        using Base = std::underlying_type_t<Flags>;
        lhs = lhs & rhs;
        return lhs;
    }

    inline bool test(Flags lhs, Flags rhs) noexcept {
        return static_cast<bool>(lhs & rhs);
    }
}

#endif