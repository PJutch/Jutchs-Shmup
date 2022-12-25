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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <ranges>
#include <algorithm>
#include <memory>
#include <utility>

GameState::GameState(sf::Vector2f screenSize) : 
        m_randomEngine{std::random_device{}()},
        m_assetManager{m_randomEngine}, m_landManager{*this},
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_scoredX{0.f}, m_shouldResetAfter{sf::Time::Zero},
        m_shouldEnd{false}, m_guiManager{*this} {
    initPlayer();
    m_landManager.init();      

    m_languageManager.setLanguage(LanguageManager::Language::ENGLISH);   
    m_guiManager.initGui();
}

void GameState::initPlayer() {
    using enum Airplane::Flags;

    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3)
        .flags(PLAYER_SIDE | HEAVY | SLOW | NO_WEAPON | UNIQUE | USE_PICKUPS | NO_BOMB)
        .shootComponent<Airplane::BasicShootComponent>()
        .shootControlComponent<Airplane::PlayerShootControlComponent>()
        .moveComponent<Airplane::PlayerMoveComponent>().speed({250.f, 250.f})
        .bombComponent<Airplane::PlayerBombComponent>()
        .addDeathEffect<Airplane::LoseDeathEffect>()
        .addDeathEffect<Airplane::ExplosionDeathEffect>()
        .build().release();
    m_entityManager.addEntity(m_player);
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
    m_landManager.update();

    checkReset(elapsedTime);
    checkEnemySpawn();
    updateScore();
}

void GameState::updateScore() {
    while (m_player->getPosition().x > m_scoredX) {
        m_scoredX += 32;
        addScore(1);
    }

    while (!m_scoreChanges.empty() && getScoreChangeAlpha(0) <= 0.f) {
        m_scoreChanges.pop_front();
    }
}

void GameState::reset() {  
    m_player->setPosition({0.f, 0.f});
    m_player->setHealth(3);

    m_entityManager.reset();

    m_spawnX = m_gameHeight * 4;

    m_landManager.reset();

    m_score = 0;
    m_scoreChanges.clear();
    m_scoredX = 0.f;

    m_clock.restart();
}

bool GameState::inActiveArea(float x) const noexcept {
    return x + 2 * getGameHeight() >= getPlayer().getPosition().x 
        && x - 5 * getGameHeight() <= getPlayer().getPosition().x;
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto prevView = target.getView();

    target.setView(getView());
    target.draw(m_landManager, states);
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
    }
}

void GameState::trySpawnEnemy(sf::Vector2f position) {
    std::uniform_real_distribution canonicalDistribution{0.0, 1.0};
    if (genRandom(canonicalDistribution) < 0.01) {
        using enum Airplane::Flags;

        Airplane::Builder builder{*this};
        builder.position(position).maxHealth(1).flags(ENEMY_SIDE | DELETABLE | NO_PICKUPS);

        int score = 10;

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.maxHealth(3);
            builder.flags() |= HEAVY;
            score *= 2;
        } else {
            builder.maxHealth(1);
            builder.flags() |= LIGHT;
        }

        double shootSeed = genRandom(canonicalDistribution);
        bool advancedWeapon = false;
        if (shootSeed < 0.1) {
            builder.shootComponent<Airplane::TripleShootComponent>();
            builder.flags() |= HAS_WEAPON;
            advancedWeapon = true;
        } else if (shootSeed < 0.2) {
            builder.shootComponent<Airplane::VolleyShootComponent>();
            builder.flags() |= NO_WEAPON;
            advancedWeapon = true;
        } else {
            builder.shootComponent<Airplane::BasicShootComponent >();
            builder.flags() |= NO_WEAPON;
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        std::unique_ptr<Airplane::ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent
                <Airplane::TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent
                <Airplane::NeverShootControlComponent>();
            builder.flags() &= ~HAS_WEAPON;
            builder.flags() |= NO_WEAPON;
            advancedWeapon = false;
            score /= 2;
        } else {
            shootControl = builder.createShootControlComponent
                <Airplane::AlwaysShootControlComponent>();
        }

        if (advancedWeapon) score *= 2;

        builder.shootControlComponent<Airplane::AndShootControlComponent>(std::move(shootControl), 
            builder.createShootControlComponent<Airplane::CanHitPlayerShootControlComponent>());

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.speed({500.f, 250.f});
            builder.flags() |= FAST;
            score *= 2;
        } else {
            builder.speed({250.f, 250.f});
            builder.flags() |= SLOW;
        }

        bool hasBomb = false;
        if (canonicalDistribution(getRandomEngine()) < 0.1) {
            builder.bomb();
            hasBomb = true;
            score *= 2;
        }
        builder.bombComponent<Airplane::EnemyBombComponent>();

        if (hasBomb && canonicalDistribution(getRandomEngine()) < 0.9) {
            builder.moveComponent<Airplane::LineWithTargetMoveComponent>(
                    [&land = getLand(), target = getLand().getTargetFor(position)] 
                    (const Airplane::Airplane& owner) mutable -> sf::Vector2f {
                        if (target.x > owner.getPosition().x && owner.hasBomb())
                            target = land.getTargetFor(owner.getPosition());
                        
                        return target;
                    });
        } else {
            double moveSeed = genRandom(canonicalDistribution);
            if (moveSeed < 0.1) {
                builder.moveComponent<Airplane::PeriodicalMoveComponent>();
            } else if (moveSeed < 0.2) {
                builder.moveComponent<Airplane::LineWithTargetMoveComponent>(
                    [&player = getPlayer()](const Airplane::Airplane&) -> sf::Vector2f {
                        return player.getPosition();
                    });
            } else {
                builder.moveComponent<Airplane::BasicMoveComponent>();
            }
        }

        builder.addDeathEffect<Airplane::ScoreDeathEffect    >(score)
               .addDeathEffect<Airplane::LootDeathEffect     >()
               .addDeathEffect<Airplane::ExplosionDeathEffect>();

        m_entityManager.addEntity(builder.build());
    }
}
