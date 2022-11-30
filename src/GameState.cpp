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
using sf::RenderTarget;
using sf::RenderStates;
using sf::Sprite;
using sf::View;
using sf::Texture;
using sf::Image;
using sf::Color;

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;
using sf::Vector2u;
using sf::Vector2f;
using sf::IntRect;

#include <format>
using std::format;

#include <random>
using std::random_device;

#include <algorithm>
using std::erase_if;
using std::max;

#include <string>
using std::string;
using std::to_string;

using std::ssize;

#include <exception>

#include <memory>
using std::unique_ptr;
using std::make_unique;

#include <utility>
using std::move;

GameState::GameState(Vector2f screenSize) : 
        m_assetManager{}, m_tickClock{}, m_clock{}, 
        m_randomEngine{random_device{}()},
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_shouldResetAfter{Time::Zero}, m_sounds{}, m_volume{100.f},
        m_shouldEnd{false}, m_guiManager{*this} {
    initPlayer();
    m_languageManager.setLanguage(LanguageManager::Language::ENGLISH);
    
    m_guiManager.initGui();
}

void GameState::initPlayer() {
    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3).deletable(false)
        .textureHeavy(true).textureFast(false).textureHasWeapon(false)
        .shootComponent<Airplane::BasicShootComponent>().playerSide(true)
        .shootControlComponent<Airplane::PlayerShootControlComponent>()
        .moveComponent<Airplane::PlayerMoveComponent>().speed({250.f, 250.f})
        .addDeathEffect<Airplane::LoseDeathEffect>()
        .addDeathEffect<Airplane::ExplosionDeathEffect>()
        .build().release();
    m_entityManager.addEntity(m_player);
}

void GameState::handleEvent(sf::Event event) {
    m_guiManager.handleEvent(event);

    if (!m_guiManager.isMenuOpen())
        m_entityManager.handleEvent(event);   

    if (event.type == sf::Event::Closed) m_shouldEnd = true;
}

void GameState::update() {
    Time elapsedTime = m_tickClock.restart();

    if (!m_guiManager.isMenuOpen()) m_entityManager.update(elapsedTime);

    erase_if(m_sounds, [this](const unique_ptr<SoundEffect>& sound) -> bool {
        return sound->hasStopped();
    });

    if (m_shouldResetAfter > Time::Zero) {
        m_shouldResetAfter -= elapsedTime;
        if (m_shouldResetAfter <= Time::Zero) reset();
    }

    while (m_player->getPosition().x + 4 * m_gameHeight > m_spawnX) {
        Vector2u enemySize = getAssets().getAirplaneTextureSize();
        for (float y = (enemySize.y - m_gameHeight) / 2; 
                y < (m_gameHeight- enemySize.y) / 2; y += enemySize.y) {
            trySpawnEnemy(Vector2f{m_spawnX, y});
        }
        m_spawnX += enemySize.x;
        addScore(1);
    }

    while (!m_scoreChanges.empty() 
            && m_scoreChanges.front().time + seconds(0.5f) < m_clock.getElapsedTime()) {
        m_score += m_scoreChanges.front().value;
        m_scoreChanges.pop_front();
    }
}

void GameState::reset() noexcept {  
    m_player->setPosition({0.f, 0.f});
    m_player->setHealth(3);

    m_entityManager.reset();

    m_spawnX = m_gameHeight * 4;

    m_score = 0;
    m_scoreChanges.clear();

    m_clock.restart();
}

bool GameState::inActiveArea(float x) const noexcept {
    return x + 2 * getGameHeight() >= getPlayer().getPosition().x 
        && x - 5 * getGameHeight() <= getPlayer().getPosition().x;
}

void GameState::draw(RenderTarget& target, RenderStates states) const {
    auto prevView = target.getView();
    target.setView(getView());

    target.draw(m_entityManager, states);

    target.setView(prevView);
    target.draw(m_guiManager, states);
}

View GameState::getView() const noexcept {
    return View{{getPlayer().getPosition().x - getGameHeight() / 2.f, 
                 -getGameHeight() / 2.f, 
                 getGameHeight() * getScreenSize().x / getScreenSize().y, 
                 getGameHeight()}};
}

void GameState::trySpawnEnemy(sf::Vector2f position) noexcept {
    std::uniform_real_distribution canonicalDistribution{0.0, 1.0};
    if (genRandom(canonicalDistribution) < 0.01) {
        Airplane::Builder builder{*this};

        builder.position(position).maxHealth(1).deletable(true).playerSide(false);

        int score = 10;

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.maxHealth(3).textureHeavy(true);
            score *= 2;
        } else {
            builder.maxHealth(1).textureHeavy(false);
        }

        double shootSeed = genRandom(canonicalDistribution);
        if (shootSeed < 0.1) {
            builder.shootComponent<Airplane::TripleShootComponent>()
                   .textureHasWeapon(true);
            score *= 2;
        } else if (shootSeed < 0.2) {
            builder.shootComponent<Airplane::VolleyShootComponent>()
                   .textureHasWeapon(false);
            score *= 2;
        } else {
            builder.shootComponent<Airplane::BasicShootComponent >()
                   .textureHasWeapon(false);
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        unique_ptr<Airplane::ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent
                <Airplane::TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent
                <Airplane::NeverShootControlComponent>();
            builder.textureHasWeapon(false);
            score /= 2;
        } else {
            shootControl = builder.createShootControlComponent
                <Airplane::AlwaysShootControlComponent>();
        }

        builder.shootControlComponent<Airplane::AndShootControlComponent>(move(shootControl), 
            builder.createShootControlComponent<Airplane::CanHitPlayerShootControlComponent>());

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.speed({500.f, 250.f}).textureFast(true);
            score *= 2;
        } else {
            builder.speed({250.f, 250.f}).textureFast(false);
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
