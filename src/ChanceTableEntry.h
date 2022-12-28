/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef CHANCE_TABLE_ENTRY_H_
#define CHANCE_TABLE_ENTRY_H_

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
}

#endif