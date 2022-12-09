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

EntityManager::EntityManager() noexcept : m_entities{} {}

void EntityManager::handleEvent(Event event) noexcept {
    for (auto& entity : m_entities) {
        if (entity->isActive()) entity->handleEvent(event);
    }
}

void EntityManager::update(Time elapsedTime) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i) 
        if (m_entities[i]->isActive()) m_entities[i]->update(elapsedTime);
    
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
        if (entity->isActive()) target.draw(*entity, states);
    }
}

void EntityManager::reset() noexcept {
    erase_if(m_entities, [](const unique_ptr<Entity>& entity) -> bool {
        return entity->reset();
    });
}
