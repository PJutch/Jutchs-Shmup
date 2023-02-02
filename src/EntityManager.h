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

#include "declarations.h"

#include "functional.h"

#include <algorithm>
#include <ranges>
#include <vector>
#include <memory>
#include <concepts>

class EntityManager : public sf::Drawable {
public:
    EntityManager(GameState& gameState) noexcept;

    void addEntity(Entity* entity) {
        m_entities.emplace_back(entity);
    }

    void addEntity(std::unique_ptr<Entity>&& entity) {
        m_entities.push_back(std::move(entity));
    }

    template <std::derived_from<Entity> EntityT, typename... Args>
    void addEntity(Args&&... args) {
        m_entities.push_back(createEntity<EntityT>(std::forward<Args>(args)...));
    }

    template <std::derived_from<Entity> EntityT, typename... Args>
    std::unique_ptr<EntityT> createEntity(Args&&... args) {
        return std::make_unique<EntityT>(m_gameState, std::forward<Args>(args)...);
    }

    // return global bounds of all entities that AI of entity must consider in pathfinding
    auto getObstaclesFor(Entity& entity) noexcept {
        return m_entities
           | std::views::filter(not_pred(indirect(&Entity::shouldBeDeleted))) 
           | std::views::filter(not_pred(indirect(&Entity::isPassable)))
           | std::views::filter([&entity](const std::unique_ptr<Entity>& obstacle) {
            return obstacle.get() != &entity;
        }) | std::views::transform(indirect(&Entity::getGlobalBounds));
    }

    sf::Vector2f getPlayerPosition() const noexcept;
    sf::FloatRect getPlayerGlobalBounds() const noexcept;
    int getPlayerHealth() const noexcept;
    
    void init();

    void handleEvent(sf::Event event) noexcept;

    void update(sf::Time elapsedTime) noexcept;

    bool trySpawnTurret(sf::Vector2f position);

    bool trySpawnTurret(float x, float y) {
        trySpawnTurret({x, y});
    }

    void reset() noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    std::vector<std::unique_ptr<Entity>> m_entities;

    Airplane::Airplane* m_player;
    sf::Vector2f m_playerPosition;
    sf::FloatRect m_playerGlobalBounds;

    float m_spawnX;

    GameState& m_gameState;

    void spawnPlayer();

    void checkEnemySpawn();
    void spawnEnemy(sf::Vector2f position);
};

#endif