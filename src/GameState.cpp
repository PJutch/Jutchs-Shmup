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

#include <SFML/System.hpp>
using sf::Time;
using sf::Vector2u;
using sf::Vector2f;

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
        m_playerTexture{}, m_healthTexture{}, m_bulletTexture{}, 
        m_enemyTexture{}, m_digitTextures{}, m_healthPickupTexture{}, 
        m_randomEngine{random_device{}()}, m_entities{}, m_player{}, 
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_shouldReset{false} {
    if (!m_playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0000.png")) 
        throw TextureLoadError{"Can't load player texture"};
    
    if (!m_healthTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0026.png"))
        throw TextureLoadError{"Can't load health texture"};
    
    if (!m_bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png"))
        throw TextureLoadError{"Can't load bullet texture"};
    
    if (!m_enemyTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png"))
        throw TextureLoadError{"Can't load enemy texture"};
    
    for (int i = 0; i < ssize(m_digitTextures); ++ i) 
        if (!m_digitTextures[i].loadFromFile(
                format("resources/kenney_pixelshmup/Digits/digit_{}.png", i))) 
            throw TextureLoadError{format("Can't load digit {} texture", i)};

    if (!m_healthPickupTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0024.png")) 
        throw TextureLoadError{"Can't load health pickup texture"};

    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3).deletable(false).texture(m_playerTexture)
        .shootComponent<BasicShootComponent>().shootRight(true)
        .shootControlComponent<PlayerShootControlComponent>()
        .moveComponent<PlayerMoveComponent>().speed({250.f, 250.f})
        .deathComponent<PlayerDeathComponent>().build().release();
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

    if (m_shouldReset)
        reset();

    while (m_player->getPosition().x + 4 * m_gameHeight > m_spawnX) {
        Vector2u enemySize = m_enemyTexture.getSize();
        for (float y = (enemySize.y - m_gameHeight) / 2; 
                y < (m_gameHeight- enemySize.y) / 2; y += enemySize.y) {
            trySpawnEnemy(Vector2f{m_spawnX, y});
        }
        m_spawnX += enemySize.x;
        addScore(1);
    }
}

void GameState::reset() noexcept {
    m_shouldReset = false;

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

    Sprite healthSprite{getHealthTexture()};
    healthSprite.scale(2, 2);
    for (int i = 0; i < getPlayer().getHealth(); ++ i) {
        healthSprite.setPosition(2 * i * getHealthTexture().getSize().x, 
                                 0.01 * getScreenSize().y);
        target.draw(healthSprite, states);
    }

    string score = to_string(m_score);
    for (int i = 0; i < ssize(score); ++ i) {
        Sprite digitSprite{getDigitTextures()[score[i] - '0']};
        digitSprite.setPosition(i * getDigitTextures()[0].getSize().x, 0);
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

        builder.position(position).maxHealth(1).deletable(true).shootRight(false);
        builder.texture(m_enemyTexture);

        double shootSeed = genRandom(canonicalDistribution);
        if (shootSeed < 0.1) {
            builder.shootComponent<TripleShootComponent>();
        } else if (shootSeed < 0.2) {
            builder.shootComponent<VolleyShootComponent>();
        } else {
            builder.shootComponent<BasicShootComponent>();
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        unique_ptr<ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent<TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent<NeverShootControlComponent>();
        } else {
            shootControl = builder.createShootControlComponent<AlwaysShootControlComponent>();
        }

        builder.shootControlComponent<AndShootControlComponent>(move(shootControl), 
            builder.createShootControlComponent<CanHitPlayerShootControlComponent>());

        builder.speed({genRandom(canonicalDistribution) < 0.1 ? 500.f : 250.f, 250.f});

        double moveSeed = genRandom(canonicalDistribution);
        if (moveSeed < 0.1) {
            builder.moveComponent<PeriodicalMoveComponent>();
        } else if (moveSeed < 0.2) {
            builder.moveComponent<FollowPlayerMoveComponent>();
        } else {
            builder.moveComponent<BasicMoveComponent>();
        }

        builder.deathComponent<LootDeathComponent>();

        addEntity(builder.build());
    }
}
