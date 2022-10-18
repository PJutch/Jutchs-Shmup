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
#include "Player.h"
#include "Enemy.h"

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

using std::ssize;

#include <memory>
using std::unique_ptr;

GameState::GameState(Vector2f screenSize) : m_playerTexture{}, m_healthTexture{}, m_bulletTexture{}, 
        m_enemyTexture{}, m_digitTextures{}, m_healthPickupTexture{}, 
        m_randomEngine{random_device{}()}, m_entities{}, m_player{}, 
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4} {
    if (!m_playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0000.png")) 
        throw TextureLoadError{"Can't load player texture"};
    
    if (!m_healthTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0026.png"))
        throw TextureLoadError{"Can't load health texture"};
    
    if (!m_bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png"))
        throw TextureLoadError{"Can't load bullet texture"};
    
    if (!m_enemyTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png"))
        throw TextureLoadError{"Can't load enemy texture"};
    
    for (int i = 0; i < ssize(m_digitTextures); ++ i) 
        if (!m_digitTextures[i].loadFromFile(format("resources/kenney_pixelshmup/Digits/digit_{}.png", i))) 
            throw TextureLoadError{format("Can't load digit {} texture", i)};

    if (!m_healthPickupTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0024.png")) 
        throw TextureLoadError{"Can't load health pickup texture"};

    m_player = Airplane::Builder<Player>{*this}.position({0.f, 0.f})
        .shootComponent<BasicShootComponent>(true)
        .shootControlComponent<PlayerShootControlComponent>().build().release();
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

    if (m_player->isDead())
        reset();

    while (m_player->getPosition().x + 4 * m_gameHeight > m_spawnX) {
        Vector2u enemySize = m_enemyTexture.getSize();
        for (float y = (enemySize.y - m_gameHeight) / 2; 
                y < (m_gameHeight- enemySize.y) / 2; y += enemySize.y) {
            Enemy::trySpawn(Vector2f{m_spawnX, y}, *this);
        }
        m_spawnX += enemySize.x;
        m_player->addScore(1);
    }
}

void GameState::reset() noexcept {
    m_player->reset();
    erase_if(m_entities, [this](const unique_ptr<Entity>& entity) -> bool {
        return entity.get() != m_player;
    });
    m_spawnX = m_gameHeight * 4;
}
