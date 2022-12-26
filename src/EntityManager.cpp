/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "EntityManager.h"

#include "GameState.h"

#include "Airplane/Airplane.h"
#include "Airplane/Builder.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;

#include <SFML/System.hpp>
using sf::Event;
using sf::Time;

#include <vector>
using std::erase_if;

#include <memory>
using std::unique_ptr;

const int PLAYER_MAX_HEALTH = 3;
const sf::Vector2f PLAYER_START_POSITION{.0f, 0.f};

EntityManager::EntityManager(GameState& gameState) noexcept : m_gameState{gameState} {}

void EntityManager::init() {
    using enum Airplane::Flags;

    m_player = Airplane::Builder{m_gameState}
        .position(PLAYER_START_POSITION).maxHealth(PLAYER_MAX_HEALTH)
        .flags(PLAYER_SIDE | HEAVY | SLOW | NO_WEAPON | UNIQUE | USE_PICKUPS | NO_BOMB)
        .shootComponent<Airplane::BasicShootComponent>()
        .shootControlComponent<Airplane::PlayerShootControlComponent>()
        .moveComponent<Airplane::PlayerMoveComponent>().speed({250.f, 250.f})
        .bombComponent<Airplane::PlayerBombComponent>()
        .addDeathEffect<Airplane::LoseDeathEffect>()
        .addDeathEffect<Airplane::ExplosionDeathEffect>()
        .build().release();
    addEntity(m_player);
}

void EntityManager::handleEvent(Event event) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i)  {
        if (m_entities[i]->isActive()) 
            m_entities[i]->handleEvent(event);
    }
}

void EntityManager::update(Time elapsedTime) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i) 
        if (m_entities[i]->isActive()) 
            m_entities[i]->update(elapsedTime);
    
    for (int i = 0; i < ssize(m_entities); ++ i) 
        for (int j = i + 1; j < ssize(m_entities); ++ j) 
            if (m_entities[i]->isActive() && m_entities[j]->isActive() && 
                    m_entities[i]->getGlobalBounds().intersects(m_entities[j]->getGlobalBounds())) {
                m_entities[i]->startCollide(*m_entities[j]);
                m_entities[j]->startCollide(*m_entities[i]);
    }

    erase_if(m_entities, [this](const unique_ptr<Entity>& entity) -> bool {
        return entity->shouldBeDeleted();
    });
}

void EntityManager::draw(RenderTarget& target, RenderStates states) const noexcept {
    for (const auto& entity : m_entities) {
        if (entity->isActive()) 
            target.draw(*entity, states);
    }
}

void EntityManager::reset() noexcept {
    erase_if(m_entities, [](const unique_ptr<Entity>& entity) -> bool {
        return entity->reset();
    });

    m_player->setPosition(PLAYER_START_POSITION);
    m_player->setHealth(PLAYER_MAX_HEALTH);
}
