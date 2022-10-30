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

#include "Airplane.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Sprite;
using sf::View;
using sf::Texture;
using sf::Image;

#include <SFML/System.hpp>
using sf::Time;
using sf::Vector2u;
using sf::Vector2f;
using sf::IntRect;

#include <format>
using std::format;

#include <random>
using std::random_device;

#include <algorithm>
using std::erase_if;

#include <string>
using std::string;
using std::to_string;

using std::ssize;

#include <memory>
using std::unique_ptr;

#include <utility>
using std::move;

GameState::GameState(Vector2f screenSize) : 
        m_assetManager{}, m_randomEngine{random_device{}()}, m_entities{}, m_player{}, 
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_shouldResetAfter{Time::Zero}, m_sounds{} {
    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3).deletable(false)
        .textureHeavy(true).textureFast(false).textureHasWeapon(false)
        .shootComponent<BasicShootComponent>().playerSide(true)
        .shootControlComponent<PlayerShootControlComponent>()
        .moveComponent<PlayerMoveComponent>().speed({250.f, 250.f})
        .addDeathEffect<LoseDeathEffect>().addDeathEffect<ExplosionDeathEffect>().build().release();
    m_entities.emplace_back(m_player);
}

void GameState::update(Time elapsedTime) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i) 
        m_entities[i]->update(elapsedTime);
    
    for (int i = 0; i < ssize(m_entities); ++ i) 
        for (int j = i + 1; j < ssize(m_entities); ++ j) 
            if (m_entities[i]->getGlobalBounds().intersects(m_entities[j]->getGlobalBounds())) 
                m_entities[i]->startCollide(*m_entities[j]);

    erase_if(m_entities, [this](const unique_ptr<Entity>& entity) -> bool {
        return entity->shouldBeDeleted();
    });

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
}

void GameState::reset() noexcept {
    m_player->setPosition({0.f, 0.f});
    m_player->setHealth(3);

    erase_if(m_entities, [this](const unique_ptr<Entity>& entity) -> bool {
        return entity.get() != m_player;
    });
    m_spawnX = m_gameHeight * 4;

    m_score = 0;
}

bool GameState::inActiveArea(float x) const noexcept {
    return x + 2 * getGameHeight() >= getPlayer().getPosition().x 
        && x - 5 * getGameHeight() <= getPlayer().getPosition().x;
}

void GameState::draw(RenderTarget& target, RenderStates states) const noexcept {
    auto prevView = target.getView();
    target.setView(getView());

    for (const auto& entity : getEntities()) {
        target.draw(*entity, states);
    }

    target.setView(prevView);

    Sprite healthSprite{getAssets().getHealthTexture()};
    healthSprite.scale(2, 2);
    for (int i = 0; i < getPlayer().getHealth(); ++ i) {
        healthSprite.setPosition(2 * i * getAssets().getHealthTexture().getSize().x, 
                                 0.01 * getScreenSize().y);
        target.draw(healthSprite, states);
    }

    string score = to_string(m_score);
    for (int i = 0; i < ssize(score); ++ i) {
        Sprite digitSprite{getAssets().getDigitTextures()[score[i] - '0']};
        digitSprite.setPosition(i * getAssets().getDigitTextures()[0].getSize().x, 0);
        target.draw(digitSprite, states);
    }
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
            builder.shootComponent<TripleShootComponent>()
                   .textureHasWeapon(true);
            score *= 2;
        } else if (shootSeed < 0.2) {
            builder.shootComponent<VolleyShootComponent>()
                   .textureHasWeapon(false);
            score *= 2;
        } else {
            builder.shootComponent<BasicShootComponent >()
                   .textureHasWeapon(false);
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        unique_ptr<ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent<TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent<NeverShootControlComponent>();
            builder.textureHasWeapon(false);
            score /= 2;
        } else {
            shootControl = builder.createShootControlComponent<AlwaysShootControlComponent>();
        }

        builder.shootControlComponent<AndShootControlComponent>(move(shootControl), 
            builder.createShootControlComponent<CanHitPlayerShootControlComponent>());

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.speed({500.f, 250.f}).textureFast(true);
            score *= 2;
        } else {
            builder.speed({250.f, 250.f}).textureFast(false);
        }

        double moveSeed = genRandom(canonicalDistribution);
        if (moveSeed < 0.1) {
            builder.moveComponent<PeriodicalMoveComponent>();
        } else if (moveSeed < 0.2) {
            builder.moveComponent<FollowPlayerMoveComponent>();
        } else {
            builder.moveComponent<BasicMoveComponent>();
        }

        builder.addDeathEffect<ScoreDeathEffect>(score)
               .addDeathEffect<LootDeathEffect>()
               .addDeathEffect<ExplosionDeathEffect>();

        addEntity(builder.build());
    }
}
