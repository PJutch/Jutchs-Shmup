/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "GameState.h"

#include "Airplane/Airplane.h"
#include "Airplane/Builder.h"
#include "Gui/Text.h"
#include "Gui/Button.h"
#include "Gui/HorizontalSlider.h"
#include "Gui/ComboBox.h"
#include "LandType.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <ranges>
#include <algorithm>
#include <memory>
#include <utility>

GameState::GameState(sf::Vector2f screenSize) : 
        m_randomEngine{std::random_device{}()},
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_shouldResetAfter{sf::Time::Zero},
        m_shouldEnd{false}, m_guiManager{*this} {
    initPlayer();
    initLand();        

    m_languageManager.setLanguage(LanguageManager::Language::ENGLISH);   
    m_guiManager.initGui();
}

void GameState::initPlayer() {
    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3)
        .flags(Airplane::Flags::PLAYER_SIDE
             | Airplane::Flags::HEAVY
             | Airplane::Flags::SLOW    
             | Airplane::Flags::NO_WEAPON
             | Airplane::Flags::UNIQUE
             | Airplane::Flags::USE_PICKUPS)
        .shootComponent<Airplane::BasicShootComponent>()
        .shootControlComponent<Airplane::PlayerShootControlComponent>()
        .moveComponent<Airplane::PlayerMoveComponent>().speed({250.f, 250.f})
        .addDeathEffect<Airplane::LoseDeathEffect>()
        .addDeathEffect<Airplane::ExplosionDeathEffect>()
        .build().release();
    m_entityManager.addEntity(m_player);
}

const std::array<double, 5> GameState::s_roadChances {0.0, 0.05, 1.0, 0.3, 0.3};
const std::array<double, 5> GameState::s_waterChances{0.0, 1.0,  0.5, 0.0, 0.3};

void GameState::prepareLandChances() {
    m_landChances.reserve(LandType::TOTAL_VARIANTS);

    for (LandType::Base i = 1; i < LandType::ROAD_VARIANTS; ++ i) {
        auto type = LandType::ROAD | static_cast<LandType>(i);
        if (type.isValid()) {
            m_landChances.emplace_back(type, 
                s_roadChances[type.getActiveDirCount()]);
            if (type.isModifiable())
                m_landChances.emplace_back(type | LandType::MODIFIED, 
                    s_roadChances[type.getActiveDirCount()]);
        }
    }

    for (LandType::Base i = 1; i < LandType::WATER_VARIANTS; ++ i) {
        auto type = LandType::WATER | static_cast<LandType>(i);
        if (type.isValid()) {
            m_landChances.emplace_back(type, 
                s_waterChances[type.getActiveDirCount()]);
            if (type.isModifiable())
                m_landChances.emplace_back(type | LandType::MODIFIED, 
                    s_waterChances[type.getActiveDirCount()]);
        }
    }

    ChanceTable::normalize(m_landChances);

    m_landChances.emplace_back(LandType::FEATURE | LandType::CRATER, 0.25);
    m_landChances.emplace_back(LandType::FEATURE | LandType::FIELD , 0.25);
    m_landChances.emplace_back(LandType::FEATURE | LandType::FLAG  , 0.25);
    m_landChances.emplace_back(LandType::FEATURE | LandType::BUSH  , 0.25);
    m_landChances.emplace_back(LandType::FEATURE | LandType::HOUSE , 0.25);
    // tree generation chance is bigger
    m_landChances.emplace_back(LandType::FEATURE | LandType::TREE  , 0.25);
    m_landChances.emplace_back(LandType::FEATURE | LandType::TREES , 0.25);
    ChanceTable::normalize(m_landChances);

    m_landChances.emplace_back(LandType::PLAINS , 10.0);
    m_landChances.emplace_back(LandType::PLAINS2, 10.0);
    m_landChances.emplace_back(LandType::WATER  , 10.0);
    m_landChances.emplace_back(LandType::ISLANDS,  0.1);
    ChanceTable::normalize(m_landChances);

    int size = std::ssize(m_landChances);
    for (int i = 0; i < size; ++ i) {
        auto val = value(m_landChances[i]);
        if (val != (LandType::FEATURE | LandType::BUSH) && val != LandType::WATER)
            m_landChances.emplace_back(value(m_landChances[i]) | LandType::BADLAND, 
                                       chance(m_landChances[i]));
    }
    ChanceTable::normalize(m_landChances);
}

void GameState::initLand() {
    prepareLandChances();

    auto playerPos = getPlayer().getPosition();
    auto tileSize = getAssets().getLandTextureSize();

    m_land.emplace_back();
    m_landEnd = playerPos.x - getGameHeight();

    float y = -getGameHeight();
    m_land.back().push_back(ChanceTable::getRandom(m_landChances, m_randomEngine));
    y += tileSize.y;

    while (y < getGameHeight()) {
        m_land.back().push_back(ChanceTable::getRandom(
                m_landChances 
                    | std::views::filter(
                        [up = m_land.back().back()] 
                        (const auto& entry) -> bool {
                            return isCompatableVertical(up, value(entry));
                })  | ChanceTable::views::normalize, 
            m_randomEngine));
        y += tileSize.y;
    }

    while (m_landEnd < playerPos.x + 5 * getGameHeight())
        addLandRow();
}

void GameState::updateLand() {
    auto playerPos = getPlayer().getPosition();
    while (playerPos.x + 5 * getGameHeight() >= m_landEnd) {
        m_land.pop_front();
        addLandRow();
    }
}

void GameState::addLandRow() {
    m_land.emplace_back();
    const auto& prevRow = m_land[std::ssize(m_land) - 2];
    auto& row = m_land.back();
    row.reserve(std::ssize(prevRow));

    row.push_back(ChanceTable::getRandom(
            m_landChances 
                | std::views::filter(
                    [left    = prevRow[0],
                    downLeft = prevRow[1]] 
                    (const auto& entry) -> bool {
                    return isCompatableHorizontal(left, value(entry))
                        && isCompatableAntiDiagonal(downLeft, value(entry));
            })  | ChanceTable::views::normalize, 
        m_randomEngine));

    while (std::ssize(row) < std::ssize(prevRow) - 1)
        row.push_back(ChanceTable::getRandom(
                m_landChances 
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
            m_randomEngine));

    row.push_back(ChanceTable::getRandom(
            m_landChances 
                | std::views::filter(
                   [up   = row    .back(), 
                    left = prevRow.back(),
                    upLeft = prevRow[std::ssize(row) - 1]] 
                    (const auto& entry) -> bool {
                        return isCompatableVertical  (up    , value(entry))
                            && isCompatableHorizontal(left  , value(entry))
                            && isCompatableDiagonal  (upLeft, value(entry));
            })  | ChanceTable::views::normalize, 
        m_randomEngine));
    
    m_landEnd += getAssets().getLandTextureSize().x;
}

void GameState::handleEvent(const sf::Event& event) {
    m_guiManager.handleEvent(event);

    if (!m_guiManager.isMenuOpen())
        m_entityManager.handleEvent(event);   

    if (event.type == sf::Event::Closed) m_shouldEnd = true;
}

void GameState::update() {
    sf::Time elapsedTime = m_tickClock.restart();

    m_soundManager.update();

    if (m_guiManager.isMenuOpen()) return;

    m_entityManager.update(elapsedTime);
    updateLand();

    checkReset(elapsedTime);
    checkEnemySpawn();
    updateScore();
}

void GameState::updateScore() {
    while (!m_scoreChanges.empty() && getScoreChangeAlpha(0) <= 0.f) {
        m_score += m_scoreChanges.front().value;
        m_scoreChanges.pop_front();
    }
}

void GameState::reset() {  
    m_player->setPosition({0.f, 0.f});
    m_player->setHealth(3);

    m_entityManager.reset();

    m_spawnX = m_gameHeight * 4;

    m_land.clear();
    initLand();

    m_score = 0;
    m_scoreChanges.clear();

    m_clock.restart();
}

bool GameState::inActiveArea(float x) const noexcept {
    return x + 2 * getGameHeight() >= getPlayer().getPosition().x 
        && x - 5 * getGameHeight() <= getPlayer().getPosition().x;
}

void GameState::drawLand(sf::RenderTarget& target, sf::RenderStates states) const {
    auto textureSize = getAssets().getLandTextureSize();
    auto playerPos = getPlayer().getPosition();

    sf::Vector2f start{playerPos.x - getGameHeight() - std::fmodf(playerPos.x, textureSize.x), 
                       -getGameHeight()};
    for (int ix = 0; start.x + ix * textureSize.x < playerPos.x + 4 * getGameHeight(); ++ ix)
        for (float iy = 0; start.y + iy * textureSize.y < getGameHeight(); ++ iy) {
            sf::Sprite sprite{getAssets().getLandTexture(m_land[ix][iy])};
            sprite.setPosition(start.x + ix * textureSize.x, start.y + iy * textureSize.y);
            target.draw(sprite, states);
    }
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto prevView = target.getView();

    target.setView(getView());
    drawLand(target, states);
    target.draw(m_entityManager, states);

    target.setView(prevView);
    target.draw(m_guiManager, states);
}

sf::View GameState::getView() const noexcept {
    return sf::View{{getPlayer().getPosition().x - getGameHeight() / 2.f, 
                     -getGameHeight() / 2.f, 
                     getGameHeight() * getScreenSize().x / getScreenSize().y, 
                     getGameHeight()}};
}

void GameState::checkEnemySpawn() {
    while (m_player->getPosition().x + 4 * m_gameHeight > m_spawnX) {
        sf::Vector2u enemySize = getAssets().getAirplaneTextureSize();
        for (float y = (enemySize.y - m_gameHeight) / 2; 
                y < (m_gameHeight- enemySize.y) / 2; y += enemySize.y) {
            trySpawnEnemy(sf::Vector2f{m_spawnX, y});
        }
        m_spawnX += enemySize.x;
        addScore(1);
    }
}

void GameState::trySpawnEnemy(sf::Vector2f position) {

    std::uniform_real_distribution canonicalDistribution{0.0, 1.0};
    if (genRandom(canonicalDistribution) < 0.01) {
        Airplane::Builder builder{*this};

        builder.position(position).maxHealth(1).flags(Airplane::Flags::ENEMY_SIDE 
                                                    | Airplane::Flags::UNIQUE
                                                    | Airplane::Flags::DELETABLE);

        int score = 10;

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.maxHealth(3);
            builder.flags() |= Airplane::Flags::HEAVY;
            score *= 2;
        } else {
            builder.maxHealth(1);
            builder.flags() |= Airplane::Flags::LIGHT;
        }

        double shootSeed = genRandom(canonicalDistribution);
        if (shootSeed < 0.1) {
            builder.shootComponent<Airplane::TripleShootComponent>();
            builder.flags() |= Airplane::Flags::HAS_WEAPON;
            score *= 2;
        } else if (shootSeed < 0.2) {
            builder.shootComponent<Airplane::VolleyShootComponent>();
            builder.flags() |= Airplane::Flags::NO_WEAPON;
            score *= 2;
        } else {
            builder.shootComponent<Airplane::BasicShootComponent >();
            builder.flags() |= Airplane::Flags::NO_WEAPON;
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        std::unique_ptr<Airplane::ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent
                <Airplane::TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent
                <Airplane::NeverShootControlComponent>();
            builder.flags() &= ~Airplane::Flags::HAS_WEAPON;
            builder.flags() |= Airplane::Flags::NO_WEAPON;
            score /= 2;
        } else {
            shootControl = builder.createShootControlComponent
                <Airplane::AlwaysShootControlComponent>();
        }

        builder.shootControlComponent<Airplane::AndShootControlComponent>(std::move(shootControl), 
            builder.createShootControlComponent<Airplane::CanHitPlayerShootControlComponent>());

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.speed({500.f, 250.f});
            builder.flags() |= Airplane::Flags::FAST;
            score *= 2;
        } else {
            builder.speed({250.f, 250.f});
            builder.flags() |= Airplane::Flags::SLOW;
        }

        double moveSeed = genRandom(canonicalDistribution);
        if (moveSeed < 0.1) {
            builder.moveComponent<Airplane::PeriodicalMoveComponent>();
        } else if (moveSeed < 0.2) {
            builder.moveComponent<Airplane::FollowPlayerMoveComponent>();
        } else {
            builder.moveComponent<Airplane::BasicMoveComponent>();
        }

        builder.addDeathEffect<Airplane::ScoreDeathEffect>(score)
               .addDeathEffect<Airplane::LootDeathEffect>()
               .addDeathEffect<Airplane::ExplosionDeathEffect>();

        m_entityManager.addEntity(builder.build());
    }
}
