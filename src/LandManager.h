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

#include "Land.h"

#include "ChanceTableEntry.h"

#include <SFML/Graphics.hpp>

#include <deque>

class GameState;

class LandManager : public sf::Drawable {
public:
    LandManager(GameState& gameState) noexcept;

    void init();

    void handleExplosion(sf::Vector2f position);

    void update();

    void reset();

    Land& operator[] (sf::Vector2f position) noexcept {
        auto [x, y] = toIndices(position);
        return m_land[x][y];
    }

    const Land& operator[] (sf::Vector2f position) const noexcept {
        auto [x, y] = toIndices(position);
        return m_land[x][y];
    }

    bool isXValid(float x) const noexcept;
    bool isPosValid(sf::Vector2f position) const noexcept;

    sf::Vector2f getTargetFor(sf::Vector2f enemyPosition) noexcept;
private:
    std::deque<std::vector<Land>> m_land;
    float m_endX;

    // targets are sorted by X
    std::vector<sf::Vector2f> m_targets;

    std::vector<ChanceTable::BasicEntry<Land>> m_chances;
    const static std::array<double, 5> s_roadChances ; // index is activeDirCount
    const static std::array<double, 5> s_waterChances; // index is activeDirCount

    GameState& m_gameState;

    void registerFeature(Land feature, double chance) {
        m_chances.emplace_back(feature, chance);
        if (feature != Land::BUSH) 
            m_chances.emplace_back(feature | Land::BADLAND, chance);
    }

    void prepareChances();
    void generateSpawn();

    void addTile(Land land);

    void addRow();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Vector2i toIndices(sf::Vector2f position) const noexcept;
};

#endif