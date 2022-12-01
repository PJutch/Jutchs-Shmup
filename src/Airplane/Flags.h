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
    // bitmasks
    struct Flags {
        enum Masks : uint8_t {
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

        const static int TEXTURE_VARIANTS = static_cast<int>(TEXTURE) + 1;

        uint8_t m_flags;

        Flags() = default;
        Flags(uint8_t flags) : m_flags{flags} {}
        Flags(Masks flags) : m_flags{flags} {}

        explicit operator bool() noexcept {
            return m_flags != 0;
        }

        explicit operator uint8_t() noexcept {
            return m_flags;
        }

        Flags operator ~ () noexcept {
            return ~m_flags;
        }

        friend Flags operator | (Flags lhs, Flags rhs) noexcept {
            return lhs.m_flags | rhs.m_flags;
        }

        Flags& operator |= (Flags flags) noexcept {
            m_flags |= flags.m_flags;
            return *this;
        }

        friend Flags operator & (Flags lhs, Flags rhs) noexcept {
            return lhs.m_flags & rhs.m_flags;
        }

        Flags& operator &= (Flags flags) noexcept {
            m_flags &= flags.m_flags;
            return *this;
        }
    };
}

#endif