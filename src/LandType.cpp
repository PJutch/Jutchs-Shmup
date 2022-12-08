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
    switch (static_cast<Masks>(*this & ~MODIFIED & ~BADLAND)) {
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
    } else if ((*this & ~FEATURE & ~MODIFIED & ~BADLAND) == BUSH) {
        return !(*this & BADLAND) && !(*this & MODIFIED);
    } else
        return static_cast<Base>(*this & ~FEATURE & ~MODIFIED & ~BADLAND) < FEATURE_VARIANTS && 
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
            if ((*this & BADLAND)) {
                return "water_islands_grass.png";
            } else {
                return "water_islands_badland.png";
            }
        } else 
            return "water.png";
    } else {
        std::filesystem::path fileName = ((*this & BADLAND) ? "badland" : "grass");

        if (*this & ROAD || *this & WATER) {
            if (*this & ROAD) {
                fileName += "_road";
            } else
                fileName += "_water";

            if (*this & NORTH)    fileName += 'N';
            if (*this & SOUTH)    fileName += 'S';
            if (*this & EAST)     fileName += 'E';
            if (*this & WEST)     fileName += 'W';

            if (*this & MODIFIED)
                if (*this & ROAD) {
                    fileName += "2";
                } else
                    fileName += "small";
        } else
            switch (static_cast<Masks>(*this & ~BADLAND)) {
                case AIRDROME:  fileName +=  "_airdrome" ; break;
                case CRATER:    fileName +=  "_crater"   ; break;
                case FIELD:     fileName +=  "_field"    ; break;
                case FLAG:      fileName +=  "_flag"     ; break;
                case TREE:      fileName +=  "_tree"     ; break;
                case BUSH:      fileName +=  "_bush"     ; break;
                case HOUSE:     fileName +=  "_house"    ; break;
                case PLAINS2:   fileName +=  "2"         ; break;
                case TREES:     fileName +=  "_trees"    ; break;
                case LOW_HOUSE: fileName +=  "_house_low"; break;
            }
        
        fileName += ".png";
        return fileName;
    }
}

std::string LandType::getName() const {
    if (*this & WATER && !(*this & DIR_MASK)) {
        if (*this & MODIFIED) {
            if ((*this & BADLAND)) {
                return "islands (grass)";
            } else {
                return "islands (badland)";
            }
        } else 
            return "water";
    } else {
        std::string name = "";

        if (*this & ROAD || *this & WATER) {
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
        } else switch (static_cast<Masks>(m_type)) {
            case PLAINS   : name += "plains"   ; break;
            case AIRDROME : name += "airdrome" ; break;
            case CRATER   : name += "crater"   ; break;
            case FIELD    : name += "field"    ; break;
            case FLAG     : name += "flag"     ; break;
            case TREE     : name += "tree"     ; break;
            case BUSH     : name += "bush"     ; break;
            case HOUSE    : name += "house"    ; break;
            case PLAINS2  : name += "plains2"  ; break;
            case TREES    : name += "trees"    ; break;
            case LOW_HOUSE: name += "low house"; break;
            default       : name += "invalid"  ; break;
        }

        if ((*this & BADLAND)) {
            name += " (grass)";
        } else {
            name += " (badland)";
        }
        return name;
    }
}

bool isCompatableHorizontal(LandType left, LandType right) {
    if (left.hasRoadSide(LandType::EAST) != right.hasRoadSide(LandType::WEST))
        return false;
    
    if (left.hasRoadSide(LandType::NORTH) && right.hasRoadSide(LandType::NORTH)
     || left.hasRoadSide(LandType::SOUTH) && right.hasRoadSide(LandType::SOUTH))
        return false;

    bool leftWater  = left .hasWaterSide(LandType::EAST);
    bool rightWater = right.hasWaterSide(LandType::WEST);
    if (leftWater != rightWater)
        return false;
    
    if (!leftWater && static_cast<bool>(left  & LandType::BADLAND) 
                   != static_cast<bool>(right & LandType::BADLAND))
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

    bool upWater   = up  .hasWaterSide(LandType::SOUTH);
    bool downWater = down.hasWaterSide(LandType::NORTH);
    if (upWater != downWater)
        return false;
    
    if (!upWater && static_cast<bool>(up   & LandType::BADLAND) 
                 != static_cast<bool>(down & LandType::BADLAND))
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

    if (!downLeft.hasWaterSide(LandType::EAST) && !upRight.hasWaterSide(LandType::SOUTH)
     &&    static_cast<bool>(downLeft & LandType::BADLAND) 
        != static_cast<bool>(upRight  & LandType::BADLAND))
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
