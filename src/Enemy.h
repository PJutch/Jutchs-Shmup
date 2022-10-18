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
#include "ShootComponent.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <random>

class Enemy : public Airplane {
public:
    Enemy(GameState& gameState) noexcept;

    static void trySpawn(sf::Vector2f position, GameState& gameState) noexcept {
        std::uniform_real_distribution canonicalDistribution{0.0, 1.0};
        if (gameState.genRandom(canonicalDistribution) < 0.01) {
            Airplane::Builder<Enemy> builder{gameState};
            builder.position(position);

            double value = gameState.genRandom(canonicalDistribution);
            if (value < 0.1) {
                builder.shootComponent<TripleShootComponent>(false);
            } else if (value < 0.2) {
                builder.shootComponent<VolleyShootComponent>(false);
            } else {
                builder.shootComponent<BasicShootComponent>(false);
            }

            builder.shootControlComponent<BasicShootControlComponent>();

            gameState.addEntity(builder.build());
        }
    }

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive || m_sprite.getPosition().x + 2 * m_gameState.getGameHeight() 
                            < m_gameState.getPlayer().getPosition().x 
                        || m_sprite.getPosition().x - 5 * m_gameState.getGameHeight()  
                            > m_gameState.getPlayer().getPosition().x;
    }

    void handleDamaged() noexcept override {
        m_alive = false;
        m_gameState.getPlayer().addScore(10);
        if (m_gameState.genRandom(std::uniform_real_distribution{0.0, 1.0}) < 0.1) {
            m_gameState.addEntity(new HealthPickup{m_sprite.getPosition(), m_gameState});
        }
    }

    bool addHealth(int health) noexcept override {
        return false;
    }
private:
    bool m_alive;
};

#endif