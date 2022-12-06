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

bool LandType::isModifiable() const noexcept {
    switch (static_cast<Masks>(*this & ~MODIFIED)) {
        case static_cast<Masks>(FEATURE | PLAINS):
        case static_cast<Masks>(FEATURE | TREE):
        case static_cast<Masks>(FEATURE | HOUSE):
        case static_cast<Masks>(ROAD | NORTH | SOUTH):
        case static_cast<Masks>(ROAD | EAST  | WEST):
            return true;
        default:
            return false;
    }
}

bool LandType::isValid() const noexcept {
    if (*this & ROAD) {
        return static_cast<Base>(*this & ~ROAD & ~MODIFIED) < ROAD_VARIANTS
            && (*this & ~ROAD & ~MODIFIED)
            && (!(*this & MODIFIED) || isModifiable());
    } else
        return static_cast<Base>(*this & ~FEATURE & ~MODIFIED) < FEATURE_VARIANTS && 
            (!(*this & MODIFIED) || isModifiable());
}

std::filesystem::path LandType::getTextureFileName() const {
    if (*this & ROAD) {
        std::filesystem::path fileName = "grass_road";
        if (*this & NORTH)    fileName += 'N';
        if (*this & SOUTH)    fileName += 'S';
        if (*this & EAST)     fileName += 'E';
        if (*this & WEST)     fileName += 'W';
        if (*this & MODIFIED) fileName += '2';
        fileName += ".png";
        return fileName;
    } else
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
    if (*this & ROAD) {
        std::string name = "";
        if (*this & NORTH)    name += "north ";
        if (*this & SOUTH)    name += "south ";
        if (*this & EAST)     name += "east ";
        if (*this & WEST)     name += "west ";
        if (*this & MODIFIED) name += "modified ";
        name += "road (grass)";
        return name;
    } else
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

bool isCompatableHorizontal(LandType left, LandType right) {
    if (left & LandType::ROAD) {
        if (right & LandType::ROAD) {
            return (    (left & LandType::EAST) &&  (right & LandType::WEST)
                    || !(left & LandType::EAST) && !(right & LandType::WEST))
                && !(left & LandType::NORTH && right & LandType::NORTH)
                && !(left & LandType::SOUTH && right & LandType::SOUTH);
        } else {
            return !(left & LandType::EAST); 
        }
    } else {
        if (right & LandType::ROAD) {
            return !(right & LandType::WEST); 
        } else {
            return true;
        }
    }
}

bool isCompatableVertical(LandType up, LandType down) {
    if (up & LandType::ROAD) {
        if (down & LandType::ROAD) {
            return (    (up & LandType::SOUTH) &&  (down & LandType::NORTH)
                    || !(up & LandType::SOUTH) && !(down & LandType::NORTH))
                && !(up & LandType::EAST && down & LandType::EAST)
                && !(up & LandType::WEST && down & LandType::WEST);
        } else {
            return !(up & LandType::SOUTH); 
        }
    } else {
        if (down & LandType::ROAD) {
            return !(down & LandType::NORTH); 
        } else {
            return true;
        }
    }
}

bool isCycle(LandType upLeft,   LandType upRight, 
             LandType downLeft, LandType downRight) {
    return upLeft    & LandType::ROAD && upRight   & LandType::ROAD
        && downLeft  & LandType::ROAD && downRight & LandType::ROAD

        && upLeft   & LandType::EAST && upRight   & LandType::WEST
        && downLeft & LandType::EAST && downRight & LandType::WEST

        && upLeft  & LandType::SOUTH && downLeft  & LandType::NORTH
        && upRight & LandType::SOUTH && downRight & LandType::NORTH;
}

// will become a cycle after adding downRight
bool willBeCycle(LandType upLeft,   LandType upRight, 
                 LandType downLeft) {
    return upLeft    & LandType::ROAD && upRight   & LandType::ROAD
        && downLeft  & LandType::ROAD

        && upLeft   & LandType::EAST && upRight   & LandType::WEST
        && downLeft & LandType::EAST

        && upLeft  & LandType::SOUTH && downLeft  & LandType::NORTH
        && upRight & LandType::SOUTH;
}