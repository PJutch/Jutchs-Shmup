/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_SHOOT_COMPONENT_H_
#define AIRPLANE_SHOOT_COMPONENT_H_

#include "../declarations.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Airplane {
    class ShootComponent {
    public:
        ShootComponent(Airplane& owner, GameState& gameState) noexcept :
            m_shootCooldown{sf::Time::Zero}, m_owner{owner}, m_gameState{gameState} {}
        virtual ~ShootComponent() = default;

        virtual void update(sf::Time elapsedTime) noexcept {
            m_shootCooldown -= elapsedTime;
        }

        void tryShoot() noexcept {
            if (m_shootCooldown <= sf::Time::Zero) {
                shoot();
            }
        }

        // width or height can be infinite and/or negative
        sf::FloatRect getGlobalAffectedArea() const noexcept;

        // in local coordinate space
        // respect rotation (+x is move direction)
        // width or height can be infinite and/or negative
        virtual sf::FloatRect getLocalAffectedArea() const noexcept;
    protected:
        virtual void shoot() noexcept = 0;

        void spawnBullet(sf::Vector2f offset) noexcept;

        void spawnBullet(float x, float y) noexcept {
            spawnBullet({x, y});
        }

        void spawnBullet() noexcept {
            spawnBullet({0.f, 0.f});
        }

        sf::Time m_shootCooldown;

        Airplane& m_owner;
        GameState& m_gameState;
    };
}

#endif