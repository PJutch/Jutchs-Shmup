/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ENTITY_MANAGER_H_
#define ENTITY_MANAGER_H_

#include "Entity.h"

#include <random>
#include <vector>
#include <array>
#include <concepts>
#include <memory>
#include <deque>

class GameState;

namespace Airplane {
    class Airplane;
}

class EntityManager : public sf::Drawable {
public:
    EntityManager(GameState& gameState) noexcept;

    auto cbegin() const noexcept {
        return m_entities.cbegin();
    }

    auto begin() noexcept {
        return m_entities.begin();
    }

    auto begin() const noexcept {
        return cbegin();
    }

    auto cend() const noexcept {
        return m_entities.cend();
    }

    auto end() noexcept {
        return m_entities.end();
    }

    auto end() const noexcept {
        return cend();
    }

    void addEntity(Entity* entity) noexcept {
        m_entities.emplace_back(entity);
    }

    void addEntity(std::unique_ptr<Entity>&& entity) noexcept {
        m_entities.push_back(std::move(entity));
    }

    const Airplane::Airplane& getPlayer() const noexcept {
        return *m_player;
    }

    Airplane::Airplane& getPlayer() noexcept {
        return *m_player;
    }

    void init();

    void handleEvent(sf::Event event) noexcept;

    void update(sf::Time elapsedTime) noexcept;

    void reset() noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    std::vector<std::unique_ptr<Entity>> m_entities;

    Airplane::Airplane* m_player;

    GameState& m_gameState;
};

#endif