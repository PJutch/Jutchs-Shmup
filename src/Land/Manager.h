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

#include <random>

class GameState;

namespace Land {
    class Manager : public sf::Drawable {
    public:
        Manager(GameState& gameState) noexcept;

        void init();

        void handleExplosion(sf::Vector2f position);

        void update();

        void reset();
    private:
        std::deque<std::vector<Type>> m_land;
        std::vector<sf::Vector2f> m_craters;
        float m_endX;

        std::vector<ChanceTable::BasicEntry<Type>> m_chances;
        const static std::array<double, 5> s_roadChances ; // index is activeDirCount
        const static std::array<double, 5> s_waterChances; // index is activeDirCount

        GameState& m_gameState;
        std::mt19937_64& m_randomEngine;

        void prepareChances();

        void registerFeature(Type feature, double chance) {
            m_chances.emplace_back(feature, chance);
            if (feature != Type::BUSH) 
                m_chances.emplace_back(feature | Type::BADLAND, chance);
        }

        void generateSpawn();
        void addRow();

        template <ChanceTable::ConstRange Chances>
            requires std::convertible_to<ChanceTable::RangeValue_t<Chances>, Land::Type>
        void addTile(Chances&& chances) {
            Type type = ChanceTable::getRandom(chances, m_randomEngine);
            m_land.back().push_back(type);
            if (canHaveCrater(type)
             && std::uniform_real_distribution{}(m_randomEngine) < 1.0) {
                m_craters.emplace_back(m_endX, getLastY());
            }
        }

        // get y of the last tile in the last row
        float getLastY() const;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    };
}

#endif