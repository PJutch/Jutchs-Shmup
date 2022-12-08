/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Land.h"

namespace Land {
    bool isModifiable(Type type) noexcept {
        using enum Type;
        switch (type & ~MODIFIED & ~BADLAND) {
            case FEATURE | PLAINS    :
            case FEATURE | TREE      :
            case FEATURE | HOUSE     :
            case ROAD | NORTH | SOUTH:
            case ROAD | EAST  | WEST :
            case WATER               :
            case WATER | NORTH | EAST:
            case WATER | NORTH | WEST:
            case WATER | SOUTH | EAST:
            case WATER | SOUTH | WEST:
                return true;
            default:
                return false;
        }
    }

    bool isValid(Type type) noexcept {
        using Base = std::underlying_type_t<Type>;
        using enum Type;
        if (test(type, ROAD)) {
            return static_cast<Base>(type & DIR_MASK) < ROAD_VARIANTS
                && test(type, DIR_MASK)
                && (!test(type, MODIFIED) || isModifiable(type));
        } else if (test(type, WATER)) {
            return static_cast<std::underlying_type_t<Type>>(type & DIR_MASK) < WATER_VARIANTS
                && ((type & DIR_MASK) != (NORTH | SOUTH))
                && ((type & DIR_MASK) != (WEST  | EAST ))
                && (getActiveDirCount(type) != 3)
                && (!test(type, MODIFIED) || isModifiable(type));
        } else if ((type & ~FEATURE & ~MODIFIED & ~BADLAND) == BUSH) {
            return !test(type, BADLAND) && !test(type, MODIFIED);
        } else
            return static_cast<Base>(type & ~FEATURE & ~MODIFIED & ~BADLAND) < FEATURE_VARIANTS && 
                (!test(type, MODIFIED) || isModifiable(type));
    }

    bool hasWaterAny(Type type, Type side) noexcept {
        using enum Type;

        if (!test(type, WATER)) return false;

        if (!test(type, DIR_MASK)) return true;
        if (test(type, side)) return true;

        if (test(type, MODIFIED)) return false;
        switch (side) {
            case NORTH: 
            case SOUTH: 
                return test(type, WEST) || test(type, EAST);
            case WEST: 
            case EAST: 
                return test(type, NORTH) || test(type, SOUTH);
            default:
                return false; // invalid
        }
    }

    std::filesystem::path getTextureFileName(Type type) {
        using enum Type;
        if (test(type, WATER) && !(test(type, DIR_MASK))) {
            if (test(type, MODIFIED)) {
                if (test(type, BADLAND)) {
                    return "water_islands_grass.png";
                } else {
                    return "water_islands_badland.png";
                }
            } else 
                return "water.png";
        } else {
            std::filesystem::path fileName = (test(type, BADLAND) ? "badland" : "grass");

            if (test(type, ROAD) || test(type, WATER)) {
                if (test(type, ROAD)) {
                    fileName += "_road";
                } else
                    fileName += "_water";

                if (test(type, NORTH))    fileName += 'N';
                if (test(type, SOUTH))    fileName += 'S';
                if (test(type, EAST))     fileName += 'E';
                if (test(type, WEST))     fileName += 'W';

                if (test(type, MODIFIED))
                    if (test(type, ROAD)) {
                        fileName += "2";
                    } else
                        fileName += "small";
            } else
                switch (type & ~BADLAND) {
                    case AIRDROME : fileName +=  "_airdrome" ; break;
                    case CRATER   : fileName +=  "_crater"   ; break;
                    case FIELD    : fileName +=  "_field"    ; break;
                    case FLAG     : fileName +=  "_flag"     ; break;
                    case TREE     : fileName +=  "_tree"     ; break;
                    case BUSH     : fileName +=  "_bush"     ; break;
                    case HOUSE    : fileName +=  "_house"    ; break;
                    case PLAINS2  : fileName +=  "2"         ; break;
                    case TREES    : fileName +=  "_trees"    ; break;
                    case LOW_HOUSE: fileName +=  "_house_low"; break;
                }
            
            fileName += ".png";
            return fileName;
        }
    }

    std::string getName(Type type) {
        using enum Type;

        if (test(type, WATER) && !(test(type, DIR_MASK))) {
            if (test(type, MODIFIED)) {
                if (test(type, BADLAND)) {
                    return "islands (grass)";
                } else {
                    return "islands (badland)";
                }
            } else 
                return "water";
        } else {
            std::string name = "";

            if (test(type, ROAD) || test(type, WATER)) {
                if (test(type, NORTH))    name += "north ";
                if (test(type, SOUTH))    name += "south ";
                if (test(type, EAST))     name += "east ";
                if (test(type, WEST))     name += "west ";

                if (test(type, ROAD)) {
                    if (test(type, MODIFIED)) name += "modified ";
                    name += "road";
                } else {
                    if (test(type, MODIFIED)) name += "small ";
                    name += "shore";
                }
            } else switch (type) {
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

            if (test(type, BADLAND)) {
                name += " (grass)";
            } else {
                name += " (badland)";
            }
            return name;
        }
    }

    bool isCompatableHorizontal(Type left, Type right) {
        using enum Type;

        if (hasRoadSide(left, EAST) != hasRoadSide(right, WEST))
            return false;
        
        if (hasRoadSide(left, NORTH) && hasRoadSide(right, NORTH)
        || hasRoadSide(left, SOUTH) && hasRoadSide(right, SOUTH))
            return false;

        bool leftWater  = hasWaterSide(left, EAST);
        bool rightWater = hasWaterSide(right, WEST);
        if (leftWater != rightWater)
            return false;
        
        if (!leftWater && test(left, BADLAND) != test(right, BADLAND))
            return false;

        if (   hasWaterCorner(left,  NORTH | EAST) 
            != hasWaterCorner(right, NORTH | WEST)
        ||     hasWaterCorner(left,  SOUTH | EAST) 
            != hasWaterCorner(right, SOUTH | WEST))
            return false;
        
        return true;
    }

    bool isCompatableVertical(Type up, Type down) {
        using enum Type;

        if (hasRoadSide(up, SOUTH) != hasRoadSide(down, NORTH))
            return false;
        
        if (hasRoadSide(up, EAST) && hasRoadSide(down, EAST)
         || hasRoadSide(up, WEST) && hasRoadSide(down, WEST))
            return false;

        bool upWater   = hasWaterSide(up, SOUTH);
        bool downWater = hasWaterSide(down, NORTH);
        if (upWater != downWater)
            return false;
        
        if (!upWater && test(up, BADLAND) != test(down, BADLAND))
            return false;

        if (hasWaterCorner(up, SOUTH | EAST) != hasWaterCorner(down, NORTH | EAST)
         || hasWaterCorner(up, SOUTH | WEST) != hasWaterCorner(down, NORTH | WEST))
            return false;
        
        return true;
    }

    bool isCompatableAntiDiagonal(Type downLeft, Type upRight) {
        using enum Type;

        if (hasRoadSide(downLeft, EAST) && hasWaterAny(upRight, SOUTH)
         || hasWaterAny(downLeft, EAST) && hasRoadSide(upRight, SOUTH))
            return false; // no tiles can be placed at downRight

        if (!hasWaterSide(downLeft, EAST) && !hasWaterSide(upRight, SOUTH)
         && test(downLeft, BADLAND) != test(upRight, BADLAND))
            return false; // no tiles can be placed at downRight
        
        if (hasWaterCorner(downLeft, NORTH | EAST) 
         != hasWaterCorner(upRight,  SOUTH | WEST))
            return false;

        return true;
    }

    bool isCompatableDiagonal(Type upLeft, Type downRight) {
        if (hasWaterCorner(upLeft,    Type::SOUTH | Type::EAST) 
         != hasWaterCorner(downRight, Type::NORTH | Type::WEST))
            return false;

        return true;
    }
}
