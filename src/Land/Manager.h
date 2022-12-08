/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef LAND_MANAGER_H_
#define LAND_MANAGER_H_

#include "Land/Type.h"
#include "ChanceTable.h"

#include <SFML/Graphics.hpp>

class GameState;

namespace Land {
    class Manager : public sf::Drawable {
    public:
        Manager(GameState& gameState) noexcept;

        void init();

        void update();

        void reset();
    private:
        std::deque<std::vector<Type>> m_land;
        float m_endX;

        std::vector<ChanceTable::BasicEntry<Type>> m_chances;
        const static std::array<double, 5> s_roadChances ; // index is activeDirCount
        const static std::array<double, 5> s_waterChances; // index is activeDirCount

        GameState& m_gameState;

        void registerFeature(Type feature, double chance) {
            m_chances.emplace_back(feature, chance);
            if (feature != Type::BUSH) 
                m_chances.emplace_back(feature | Type::BADLAND, chance);
        }

        void prepareChances();
        void generateSpawn();

        void addRow();

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    };
}

#endif