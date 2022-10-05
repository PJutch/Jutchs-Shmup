/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ENEMY_H_
#define ENEMY_H_

#include "Airplane.h"
#include "Entity.h"
#include "Player.h"
#include "HealthPickup.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <random>

class Enemy : public Airplane {
public:
    Enemy(sf::Vector2f position, Player& player, 
          const sf::Texture& texture, const sf::Texture& bulletTexture, 
          const sf::Texture& healthPickupTexture,
          std::vector<std::unique_ptr<Entity>>& entities, std::mt19937_64& randomEngine,
          float gameHeight) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive || m_sprite.getPosition().x + 2 * m_gameHeight < m_player.getPosition().x 
                        || m_sprite.getPosition().x - 5 * m_gameHeight > m_player.getPosition().x;
    }

    void handleDamaged() noexcept override {
        m_alive = false;
        m_player.setScore(m_player.getScore() + 10);
        if (std::uniform_real_distribution{0.0, 1.0}(m_randomEngine) < 0.1) {
            m_entities.emplace_back(new HealthPickup{m_sprite.getPosition(), m_healthPickupTexture, 
                                                     m_player, m_gameHeight});
        }
    }

    bool addHealth(int health) noexcept override {
        return false;
    }
private:
    bool m_alive;

    std::mt19937_64& m_randomEngine;

    const sf::Texture& m_healthPickupTexture;
};

#endif