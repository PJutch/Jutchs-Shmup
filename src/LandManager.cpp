/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "LandManager.h"

#include "Airplane/Airplane.h"

#include "GameState.h"

#include "ChanceTable.h"

#include <random>
#include <ranges>
#include <algorithm>

LandManager::LandManager(GameState& gameState) noexcept : m_gameState{gameState} {}

void LandManager::init() {
    prepareChances();
    startSpawnGeneration();
}

const std::array<double, 5> LandManager::s_roadChances {0.0, 0.05, 1.0, 0.3, 0.3};
const std::array<double, 5> LandManager::s_waterChances{0.0, 1.0,  0.5, 0.0, 0.3};

void LandManager::prepareChances() {
    m_chances.reserve(LAND_VARIANTS);

    forValidRoad(
        [&chances = m_chances, &roadChances = s_roadChances](Land land) {
            chances.emplace_back(land, roadChances[getActiveDirCount(land)]);
    });
    forValidWater(
        [&chances = m_chances, &waterChances = s_waterChances](Land land) {
            chances.emplace_back(land, waterChances[getActiveDirCount(land)]);
    });
    ChanceTable::normalize(m_chances);

    using enum Land;

    registerFeature(CRATER     , 0.25);
    registerFeature(FIELD      , 0.25);
    registerFeature(PLAYER_FLAG, 0.25);
    registerFeature(ENEMY_FLAG , 0.25);
    registerFeature(BUSH       , 0.25);
    registerFeature(HOUSE      , 0.25);
    // tree generation chance is bigger
    registerFeature(TREE       , 0.25);
    registerFeature(TREES      , 0.25);
    ChanceTable::normalize(m_chances);

    registerFeature(PLAINS , 10.0);
    registerFeature(PLAINS2, 10.0);
    registerFeature(ISLANDS,  0.1);
    m_chances.emplace_back(WATER, 10.0);
    ChanceTable::normalize(m_chances);
}

void LandManager::startSpawnGeneration() {
    auto tileSize = m_gameState.getAssets().getLandTextureSize();

    m_land.emplace_back();
    m_endX = -m_gameState.getGameHeight() / 2;

    float y = -m_gameState.getGameHeight() / 2;
    addTile(ChanceTable::getRandom(m_chances, m_gameState.getRandomEngine()));
    y += tileSize.y;

    while (y < m_gameState.getGameHeight() / 2) {
        addTile(ChanceTable::getRandom(
                m_chances 
                    | std::views::filter(
                        [up = m_land.back().back()] 
                        (const auto& entry) -> bool {
                            return isCompatableVertical(up, value(entry));
                })  | ChanceTable::views::normalize, 
            m_gameState.getRandomEngine()));
        y += tileSize.y;
    }
}

bool LandManager::isLoading() const {
    return m_endX < 5 * m_gameState.getGameHeight();
}

void LandManager::update() {
    float playerX = m_gameState.getEntities().getPlayerPosition().x;
    while (playerX + 5 * m_gameState.getGameHeight() >= m_endX) {
        m_land.pop_front();
        addRow();
    }

    auto tileSize = m_gameState.getAssets().getLandTextureSize();
    std::erase_if(m_targets, 
    [minX = m_endX - std::ssize(m_land) * tileSize.x](sf::Vector2f target) -> bool {
        return target.x <= minX;
    });
}

void LandManager::addRow() {
    m_land.emplace_back();
    const auto& prevRow = m_land[std::ssize(m_land) - 2];
    auto& row = m_land.back();
    row.reserve(std::ssize(prevRow));

    addTile(ChanceTable::getRandom(
            m_chances 
                | std::views::filter(
                    [left     = prevRow[0],
                        downLeft = prevRow[1]] 
                    (const auto& entry) -> bool {
                    return isCompatableHorizontal(left, value(entry))
                        && isCompatableAntiDiagonal(downLeft, value(entry));
            })  | ChanceTable::views::normalize, 
        m_gameState.getRandomEngine()));

    while (std::ssize(row) < std::ssize(prevRow) - 1)
        addTile(ChanceTable::getRandom(
                m_chances 
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
                })  | ChanceTable::views::normalize, 
            m_gameState.getRandomEngine()));

    row.push_back(ChanceTable::getRandom(
            m_chances 
                | std::views::filter(
                [up   = row    .back(), 
                    left = prevRow.back(),
                    upLeft = prevRow[std::ssize(row) - 1]] 
                    (const auto& entry) -> bool {
                        return isCompatableVertical  (up    , value(entry))
                            && isCompatableHorizontal(left  , value(entry))
                            && isCompatableDiagonal  (upLeft, value(entry));
            })  | ChanceTable::views::normalize, 
        m_gameState.getRandomEngine()));
    
    m_endX += m_gameState.getAssets().getLandTextureSize().x;
}

void LandManager::reset() {
    m_land.clear();
    startSpawnGeneration();
}

bool LandManager::isXValid(float x) const noexcept {
    auto tileSize = m_gameState.getAssets().getLandTextureSize();
    return x < m_endX && x > m_endX - std::ssize(m_land) * tileSize.x;
}

sf::Vector2i LandManager::toIndices(sf::Vector2f position) const noexcept {
    auto tileSize = m_gameState.getAssets().getLandTextureSize();
    sf::Vector2i landSize(std::ssize(m_land), std::ssize(m_land[0]));
    return sf::Vector2i(landSize.x - (m_endX - position.x) / tileSize.x, 
                        (m_gameState.getGameHeight() / 2 + position.y) / tileSize.y);
}

void LandManager::handleBombExplosion(sf::Vector2f position) {  
    if (!isXValid(position.x)) return;     
    Land& land = (*this)[position];
    m_gameState.getScoreManager().addScore(scoreIfDestroyed(land));
    land = destroyed(land);
}

void LandManager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto textureSize = m_gameState.getAssets().getLandTextureSize();
    float playerX = m_gameState.getEntities().getPlayerPosition().x;
    float gameHeight = m_gameState.getGameHeight();

    sf::Vector2f start{playerX - gameHeight / 2 - std::fmodf(playerX, textureSize.x), 
                        -gameHeight / 2};
    for (int ix = 0; start.x + ix * textureSize.x < playerX + 4 * gameHeight; ++ ix)
        for (float iy = 0; iy < std::ssize(m_land[ix]); ++ iy) {
            sf::Sprite sprite{m_gameState.getAssets().getLandTexture(m_land[ix][iy])};
            sprite.setPosition(start.x + ix * textureSize.x, start.y + iy * textureSize.y);
            target.draw(sprite, states);
    }
}

sf::Vector2f LandManager::getTargetFor(sf::Vector2f enemyPosition) noexcept {    
    auto maxIter = std::ranges::upper_bound(m_targets, enemyPosition.x, {}, &sf::Vector2f::x);
    int maxIndex = (maxIter - m_targets.begin()) - 1;

    if (maxIndex < 0)
        return enemyPosition;

    int index = std::uniform_int_distribution{0, maxIndex}(m_gameState.getRandomEngine());
    return m_targets[index];
}

void LandManager::addTile(Land land) {
    auto tileSize = m_gameState.getAssets().getLandTextureSize();
    float gameHeight = m_gameState.getGameHeight();

    m_land.back().push_back(land);

    sf::Vector2f position{m_endX + tileSize.x / 2.f, 
        (std::ssize(m_land.back()) - 1) * tileSize.y - gameHeight / 2.f + tileSize.y / 2.f};
    
    if (isEnemyTarget(land))
        m_targets.push_back(position);
    
    if (canHaveTurret(land))
        m_gameState.getEntities().trySpawnTurret(position);
}

bool LandManager::isPosValid(sf::Vector2f position) const noexcept {
    float gameHeight = m_gameState.getGameHeight();
    return isXValid(position.x) && position.y > -gameHeight / 2 && position.y < gameHeight / 2;
}
