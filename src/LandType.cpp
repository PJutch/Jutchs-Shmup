/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "LandType.h"

bool LandType::isModyfiable() const noexcept {
    switch (static_cast<Masks>(*this & ~MODIFIED)) {
        case PLAINS:
        case TREE:
        case HOUSE:
            return true;
        default:
            return false;
    }
}

std::filesystem::path LandType::getTextureFileName() const {
    switch (static_cast<Masks>(m_type)) {
    case PLAINS:    return "grass.png";
    case AIRDROME:  return "grass_airdrome.png";
    case CRATER:    return "grass_crater.png";
    case FIELD:     return "grass_field.png";
    case FLAG:      return "grass_flag.png";
    case TREE:      return "grass_tree.png";
    case BUSH:      return "grass_bush.png";
    case HOUSE:     return "grass_house.png";
    case PLAINS2:   return "grass2.png";
    case TREES:     return "grass_trees.png";
    case LOW_HOUSE: return "grass_house_low.png";
    default:        return "";
    }
}

std::string LandType::getName() const {
    switch (static_cast<Masks>(m_type)) {
    case PLAINS:    return "plains (grass)";
    case AIRDROME:  return "airdrome (grass)";
    case CRATER:    return "crater (grass)";
    case FIELD:     return "field (grass)";
    case FLAG:      return "flag (grass)";
    case TREE:      return "tree (grass)";
    case BUSH:      return "bush (grass)";
    case HOUSE:     return "house (grass)";
    case PLAINS2:   return "plains2 (grass)";
    case TREES:     return "trees (grass)";
    case LOW_HOUSE: return "low house (grass)";
    default:        return "invalid";
    }
}
