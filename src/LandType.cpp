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
        case static_cast<Masks>(FEATURE | PLAINS    ):
        case static_cast<Masks>(FEATURE | TREE      ):
        case static_cast<Masks>(FEATURE | HOUSE     ):
        case static_cast<Masks>(ROAD | NORTH | SOUTH):
        case static_cast<Masks>(ROAD | EAST  | WEST ):
        case static_cast<Masks>(WATER):
        case static_cast<Masks>(WATER | NORTH | EAST):
        case static_cast<Masks>(WATER | NORTH | WEST):
        case static_cast<Masks>(WATER | SOUTH | EAST):
        case static_cast<Masks>(WATER | SOUTH | WEST):
            return true;
        default:
            return false;
    }
}

bool LandType::isValid() const noexcept {
           if (*this & ROAD) {
        return static_cast<Base>(*this & DIR_MASK) < ROAD_VARIANTS
            && (*this & DIR_MASK)
            && (!(*this & MODIFIED) || isModifiable());
    } else if (*this & WATER) {

        return static_cast<Base>(*this & DIR_MASK) < WATER_VARIANTS
            && ((*this & DIR_MASK) != (NORTH | SOUTH))
            && ((*this & DIR_MASK) != (WEST  | EAST ))
            && (getActiveDirCount() != 3)
            && (!(*this & MODIFIED) || isModifiable());
    } else
        return static_cast<Base>(*this & ~FEATURE & ~MODIFIED) < FEATURE_VARIANTS && 
            (!(*this & MODIFIED) || isModifiable());
}

bool LandType::hasWaterAny(LandType side) const noexcept {
    if (!(*this & WATER)) return false;

    if (!(*this & DIR_MASK)) return true;
    if (*this & side) return true;

    if (*this & MODIFIED) return false;
    switch (static_cast<Masks>(side.m_type)) {
        case NORTH: 
        case SOUTH: 
            return (*this & WEST) || (*this & EAST);
        case WEST: 
        case EAST: 
            return (*this & NORTH) || (*this & SOUTH);
        default:
            return false; // invalid
    }
}

std::filesystem::path LandType::getTextureFileName() const {
    if (*this & WATER && !(*this & DIR_MASK)) {
        if (*this & MODIFIED) {
            return "water_islands_grass.png";
        } else 
            return "water.png";
    } else if (*this & ROAD || *this & WATER) {
        std::filesystem::path fileName = "grass_";
        if (*this & ROAD) {
            fileName += "road";
        } else
            fileName += "water";

        if (*this & NORTH)    fileName += 'N';
        if (*this & SOUTH)    fileName += 'S';
        if (*this & EAST)     fileName += 'E';
        if (*this & WEST)     fileName += 'W';

        if (*this & MODIFIED)
            if (*this & ROAD) {
                fileName += "2";
            } else
                fileName += "small";
        
        fileName += ".png";
        return fileName;
    } else
        switch (static_cast<Masks>(m_type)) {
            case PLAINS:    return "grass.png"          ;
            case AIRDROME:  return "grass_airdrome.png" ;
            case CRATER:    return "grass_crater.png"   ;
            case FIELD:     return "grass_field.png"    ;
            case FLAG:      return "grass_flag.png"     ;
            case TREE:      return "grass_tree.png"     ;
            case BUSH:      return "grass_bush.png"     ;
            case HOUSE:     return "grass_house.png"    ;
            case PLAINS2:   return "grass2.png"         ;
            case TREES:     return "grass_trees.png"    ;
            case LOW_HOUSE: return "grass_house_low.png";
            default:        return ""                   ;
        }
}

std::string LandType::getName() const {
    if (*this & WATER && !(*this & DIR_MASK)) {
        if (*this & MODIFIED) {
            return "islands (grass)";
        } else 
            return "water";
    } else if (*this & ROAD || *this & WATER) {
        std::string name = "";
        if (*this & NORTH)    name += "north ";
        if (*this & SOUTH)    name += "south ";
        if (*this & EAST)     name += "east ";
        if (*this & WEST)     name += "west ";
        if (*this & ROAD) {
            if (*this & MODIFIED) name += "modified ";
            name += "road";
        } else {
            if (*this & MODIFIED) name += "small ";
            name += "shore";
        }
        name += " (grass)";
        return name;
    } else switch (static_cast<Masks>(m_type)) {
        case PLAINS   : return "plains (grass)"   ;
        case AIRDROME : return "airdrome (grass)" ;
        case CRATER   : return "crater (grass)"   ;
        case FIELD    : return "field (grass)"    ;
        case FLAG     : return "flag (grass)"     ;
        case TREE     : return "tree (grass)"     ;
        case BUSH     : return "bush (grass)"     ;
        case HOUSE    : return "house (grass)"    ;
        case PLAINS2  : return "plains2 (grass)"  ;
        case TREES    : return "trees (grass)"    ;
        case LOW_HOUSE: return "low house (grass)";
        default       : return "invalid"          ;
    }
}

bool isCompatableHorizontal(LandType left, LandType right) {
    if (left.hasRoadSide(LandType::EAST) != right.hasRoadSide(LandType::WEST))
        return false;
    
    if (left.hasRoadSide(LandType::NORTH) && right.hasRoadSide(LandType::NORTH)
     || left.hasRoadSide(LandType::SOUTH) && right.hasRoadSide(LandType::SOUTH))
        return false;

    if (left.hasWaterSide(LandType::EAST) != right.hasWaterSide(LandType::WEST))
        return false;
    
    if (    left .hasWaterCorner(LandType::NORTH | LandType::EAST) 
         != right.hasWaterCorner(LandType::NORTH | LandType::WEST)
     ||     left .hasWaterCorner(LandType::SOUTH | LandType::EAST) 
         != right.hasWaterCorner(LandType::SOUTH | LandType::WEST))
        return false;
    
    return true;
}

bool isCompatableVertical(LandType up, LandType down) {
    if (up.hasRoadSide(LandType::SOUTH) != down.hasRoadSide(LandType::NORTH))
        return false;
    
    if (up.hasRoadSide(LandType::EAST) && down.hasRoadSide(LandType::EAST)
     || up.hasRoadSide(LandType::WEST) && down.hasRoadSide(LandType::WEST))
        return false;

    if (up.hasWaterSide(LandType::SOUTH) != down.hasWaterSide(LandType::NORTH))
        return false;

    if (    up  .hasWaterCorner(LandType::SOUTH | LandType::EAST) 
         != down.hasWaterCorner(LandType::NORTH | LandType::EAST)
     ||     up  .hasWaterCorner(LandType::SOUTH | LandType::WEST) 
         != down.hasWaterCorner(LandType::NORTH | LandType::WEST))
        return false;
    
    return true;
}

bool isCompatableAntiDiagonal(LandType downLeft, LandType upRight) {
    if (downLeft.hasRoadSide(LandType::EAST) && upRight.hasWaterAny(LandType::SOUTH)
     || downLeft.hasWaterAny(LandType::EAST) && upRight.hasRoadSide(LandType::SOUTH))
        return false; // no tiles can be placed at downRight
    
    if (downLeft.hasWaterCorner(LandType::NORTH | LandType::EAST) 
     != upRight .hasWaterCorner(LandType::SOUTH | LandType::WEST))
        return false;

    return true;
}

bool isCompatableDiagonal(LandType upLeft, LandType downRight) {
    if (upLeft   .hasWaterCorner(LandType::SOUTH | LandType::EAST) 
     != downRight.hasWaterCorner(LandType::NORTH | LandType::WEST))
        return false;

    return true;
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