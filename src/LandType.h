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
#include <cstdint>

// bitmask
class LandType {
public:
    using Base = uint16_t;

    // don't use directly
    enum class Masks : Base {
        PLAINS    = 0b0000,
        AIRDROME  = 0b0001,
        CRATER    = 0b0010,
        FIELD     = 0b0011,
        FLAG      = 0b0100,
        TREE      = 0b0101,
        BUSH      = 0b0110,
        HOUSE     = 0b0111,

        MODIFIED  = 0b1000, // apply to get special meaning
        PLAINS2   = PLAINS | MODIFIED,
        TREES     = TREE   | MODIFIED,
        LOW_HOUSE = HOUSE  | MODIFIED,
    };
    using enum Masks;

    const static int TOTAL_VARIANTS = 16; // WARNING: some are invalid

    LandType() = default;
    LandType(Base type) : m_type{type} {}
    LandType(Masks type) : m_type{static_cast<Base>(type)} {}

    explicit operator bool() noexcept {
        return m_type != 0;
    }

    explicit operator Base() noexcept {
        return m_type;
    }

    LandType operator ~ () noexcept {
        return ~m_type;
    }

    friend LandType operator | (LandType lhs, LandType rhs) noexcept {
        return lhs.m_type | rhs.m_type;
    }

    LandType& operator |= (LandType flags) noexcept {
        m_type |= flags.m_type;
        return *this;
    }

    friend LandType operator & (LandType lhs, LandType rhs) noexcept {
        return lhs.m_type & rhs.m_type;
    }

    LandType& operator &= (LandType flags) noexcept {
        m_type &= flags.m_type;
        return *this;
    }

    bool isModyfiable() const noexcept;

    bool isValid() const noexcept {
        return m_type < TOTAL_VARIANTS && (!(*this & MODIFIED) || isModyfiable());
    }

    // only file name, add path to search by yourself
    std::filesystem::path getTextureFileName() const;
    std::string getName() const;
private:
    Base m_type;

    explicit operator Masks() noexcept {
        return static_cast<Masks>(m_type);
    }
};

inline LandType operator ~ (LandType::Masks flags) noexcept {
    return ~static_cast<LandType>(flags);
}

inline LandType operator | (LandType::Masks lhs, LandType::Masks rhs) noexcept {
    return static_cast<LandType>(lhs) | static_cast<LandType>(rhs);
}

inline LandType operator & (LandType::Masks lhs, LandType::Masks rhs) noexcept {
    return static_cast<LandType>(lhs) & static_cast<LandType>(rhs);
}

#endif