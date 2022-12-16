/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Flags.h"

namespace Airplane {
    std::filesystem::path getTextureFileName(Flags flags) {
        std::filesystem::path path = "airplane";

        using enum Flags;
        if (test(flags, PLAYER_SIDE)) {
            path += "_player";
        } else {
            path += "_enemy";
        }

        if (test(flags, HEAVY     )) path += "_heavy" ;
        if (test(flags, FAST      )) path += "_fast"  ;
        if (test(flags, HAS_WEAPON)) path += "_weapon";
        if (test(flags, HAS_BOMB  )) path += "_bomb"  ;

        path += ".png";
        return path;
    }

    std::string getTextureName(Flags flags) {
        std::string name = "";

        using enum Flags;
        if (test(flags, PLAYER_SIDE)) {
            name = "player";
        } else {
            name = "enemy";
        }

        if (test(flags, HEAVY     )) name += " heavy" ;
        if (test(flags, FAST      )) name += " fast"  ;

        if (test(flags, HAS_WEAPON)) {
            name += "with weapon";
        } else {
            name += "without weapon";
        }

        if (test(flags, HAS_BOMB)) {
            name += "with bomb";
        } else {
            name += "without bomb";
        }

        return name;
    }
}
