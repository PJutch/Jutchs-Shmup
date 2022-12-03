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

void GameState::initLand() {
    auto playerPos = getPlayer().getPosition();
    auto tileSize = getAssets().getLandTextureSize();

    float x;
    for (x = playerPos.x - getGameHeight(); 
            x < playerPos.x + 5 * getGameHeight(); x += tileSize.x)
        m_land.push_back(createLandRow());
    
    m_landEnd = x;
}

void GameState::updateLand() {
    auto playerPos = getPlayer().getPosition();
    if (playerPos.x + 5 * getGameHeight() < m_landEnd) return;

    m_land.pop_front();
    m_land.push_back(createLandRow());
    m_landEnd += getAssets().getLandTextureSize().x; 
}

std::vector<LandType> GameState::createLandRow() {
    auto playerPos = getPlayer().getPosition();

    std::vector<LandType> row;
    for (float y = playerPos.y - getGameHeight(); y < playerPos.y + getGameHeight(); 
            y += getAssets().getLandTextureSize().y) {
        row.push_back(randomLandType());
    }
    return row;
}

LandType GameState::randomLandType() {
    double value = std::uniform_real_distribution{0.0, 1.0}(m_randomEngine);
    // Airdromes aren't generating indently
    if (value < 0.01) {
        return LandType::CRATER;
    } else if (value < 0.02) {
        return LandType::FIELD;
    } else if (value < 0.03) {
        return LandType::FLAG;
    } else if (value < 0.04) {
        return LandType::HOUSE;
    } else if (value < 0.045) {
        return LandType::LOW_HOUSE;
    } else if (value < 0.05) {
        return LandType::TREE;
    } else if (value < 0.06) {
        return LandType::TREES;
    } else if (value < 0.07) {
        return LandType::BUSH;
    } else if (value < 0.535) {
        return LandType::PLAINS;
    } else {
        return LandType::PLAINS2;
    }
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
