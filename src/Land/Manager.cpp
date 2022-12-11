/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Land/Manager.h"
#include "GameState.h"
#include "Airplane/Airplane.h"

#include <ranges>
#include <iostream>
#include <random>

namespace Land {
    Manager::Manager(GameState& gameState) noexcept : 
        m_gameState{gameState}, m_randomEngine(gameState.getRandomEngine()) {}

    void Manager::init() {
        prepareChances();
        generateSpawn();
    }

    const std::array<double, 5> Manager::s_roadChances {0.0, 0.05, 1.0, 0.3, 0.3};
    const std::array<double, 5> Manager::s_waterChances{0.0, 1.0,  0.5, 0.0, 0.3};

    void Manager::prepareChances() {
        m_chances.reserve(TOTAL_VARIANTS);

        forValidRoad(
            [&chances = m_chances, 
            &roadChances = s_roadChances] 
            (Type type) {
                chances.emplace_back(type, roadChances[getActiveDirCount(type)]);
        });

        forValidWater(
            [&chances = m_chances, 
            &waterChances = s_waterChances] 
            (Type type) {
                chances.emplace_back(type, waterChances[getActiveDirCount(type)]);
        });

        ChanceTable::normalize(m_chances);

        using enum Type;

        registerFeature(FEATURE | FIELD , 0.25);
        registerFeature(FEATURE | FLAG  , 0.25);
        registerFeature(FEATURE | BUSH  , 0.25);
        registerFeature(FEATURE | HOUSE , 0.25);
        // tree generation chance is bigger
        registerFeature(FEATURE | TREE  , 0.25);
        registerFeature(FEATURE | TREES , 0.25);
        ChanceTable::normalize(m_chances);

        registerFeature(PLAINS , 10.0);
        registerFeature(PLAINS2, 10.0);
        registerFeature(ISLANDS,  0.1);
        m_chances.emplace_back(WATER, 10.0);
        ChanceTable::normalize(m_chances);
    }

    void Manager::generateSpawn() {
        auto tileSize = m_gameState.getAssets().getLandTextureSize();

        m_land.emplace_back();
        m_endX = -m_gameState.getGameHeight();

        addTile(m_chances);
        while (getLastY() < m_gameState.getGameHeight()) {
            addTile(m_chances 
                        | std::views::filter(
                            [up = m_land.back().back()] 
                            (const auto& entry) -> bool {
                                return isCompatableVertical(up, value(entry));
                    })  | ChanceTable::views::normalize);
        }

        while (m_endX < 5 * m_gameState.getGameHeight())
            addRow();
    }

    void Manager::update() {
        auto playerPos = m_gameState.getPlayer().getPosition();
        while (playerPos.x + 5 * m_gameState.getGameHeight() >= m_endX) {
            m_land.pop_front();
            addRow();
        }

        std::erase_if(m_craters, [&gameState = m_gameState](sf::Vector2f crater) -> bool {
            return !gameState.inActiveArea(crater.x);
        });
    }

    float Manager::getLastY() const {
        auto tileSize = m_gameState.getAssets().getLandTextureSize();
        return (std::ssize(m_land.back()) - 1) * tileSize.y - m_gameState.getGameHeight();
    }

    void Manager::addRow() {
        m_land.emplace_back();
        const auto& prevRow = m_land[std::ssize(m_land) - 2];
        auto& row = m_land.back();
        row.reserve(std::ssize(prevRow));

        addTile(m_chances 
                    | std::views::filter(
                        [left     = prevRow[0],
                         downLeft = prevRow[1]] 
                        (const auto& entry) -> bool {
                        return isCompatableHorizontal(left, value(entry))
                            && isCompatableAntiDiagonal(downLeft, value(entry));
                })  | ChanceTable::views::normalize);
        while (std::ssize(row) < std::ssize(prevRow) - 1)
            addTile(m_chances 
                        | std::views::filter(
                        [up = row.back(), 
                         left     = prevRow[std::ssize(row)    ],
                         upLeft   = prevRow[std::ssize(row) - 1],
                         downLeft = prevRow[std::ssize(row) + 1]] 
                        (const auto& entry) -> bool {
                            return isCompatableVertical    (up      , value(entry))
                                && isCompatableHorizontal  (left    , value(entry))
                                && isCompatableDiagonal    (upLeft  , value(entry))
                                && isCompatableAntiDiagonal(downLeft, value(entry));
                    })  | ChanceTable::views::normalize);
        addTile(m_chances 
                    | std::views::filter(
                        [up   = row    .back(), 
                         left = prevRow.back(),
                         upLeft = prevRow[std::ssize(row) - 1]] 
                        (const auto& entry) -> bool {
                            return isCompatableVertical  (up    , value(entry))
                                && isCompatableHorizontal(left  , value(entry))
                                && isCompatableDiagonal  (upLeft, value(entry));
                })  | ChanceTable::views::normalize);
        
        m_endX += m_gameState.getAssets().getLandTextureSize().x;
    }

    void Manager::reset() {
        m_land.clear();
        generateSpawn();
    }

    void Manager::handleExplosion(sf::Vector2f position) {
        auto tileSize = m_gameState.getAssets().getLandTextureSize();
        sf::Vector2i landSize(std::ssize(m_land), std::ssize(m_land[0]));

        sf::Vector2i landPosition((position.x - m_endX) / tileSize.x + landSize.x, 
                                  (position.y + m_gameState.getGameHeight()) / tileSize.y);
        
        if (landPosition.x < 0 || landPosition.x > landSize.x) return;
        
        Land::Type& tile = m_land[landPosition.x][landPosition.y];
        tile = destroyed(tile);
        if (canHaveCrater(tile)) {
            auto craterSize = m_gameState.getAssets().getCraterTexture().getSize();
            m_craters.emplace_back(position.x - craterSize.x / 2.f, 
                                   position.y - craterSize.y / 2.f);
        }
    }

    void Manager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        auto textureSize = m_gameState.getAssets().getLandTextureSize();
        auto playerPos = m_gameState.getPlayer().getPosition();
        float gameHeight = m_gameState.getGameHeight();

        sf::Vector2f start{playerPos.x - gameHeight - std::fmodf(playerPos.x, textureSize.x), 
                           -gameHeight};
        for (int ix = 0; start.x + ix * textureSize.x < playerPos.x + 4 * gameHeight; ++ ix)
            for (float iy = 0; start.y + iy * textureSize.y < gameHeight; ++ iy) {
                sf::Sprite sprite{m_gameState.getAssets().getLandTexture(m_land[ix][iy])};
                if (sprite.getTextureRect().width == 0)
                    std::cout << static_cast<int>(m_land[ix][iy]) << std::endl;
                sprite.setPosition(start.x + ix * textureSize.x, start.y + iy * textureSize.y);
                target.draw(sprite, states);
        }

        for (auto crater : m_craters) {
            auto& texture = m_gameState.getAssets().getCraterTexture();
            sf::Sprite sprite{texture};

            sprite.setPosition(crater);
            target.draw(sprite, states);
        }
    }
}
