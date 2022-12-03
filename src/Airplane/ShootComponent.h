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

#include "Entity.h"
#include "Bullet.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <vector>
#include <memory>
#include <tuple>
#include <cmath>

namespace Airplane {
    class Airplane;

    class ShootComponent {
    public:
        ShootComponent(Airplane& owner, GameState& gameState) noexcept;
        virtual ~ShootComponent() = default;

        virtual void update(sf::Time elapsedTime) noexcept {
            m_shootCooldown -= elapsedTime;
        }

        virtual void tryShoot() noexcept = 0;

        // width or height can be infintiy
        // left or top can be negative
        virtual sf::FloatRect getAffectedArea() const noexcept;

        virtual sf::FloatRect getStartShotBounds() const noexcept;

        sf::Vector2f getShotSpeed() const noexcept;
    protected:
        void shoot(sf::Vector2f position) noexcept;

        sf::Time m_shootCooldown;

        Airplane& m_owner;
        GameState& m_gameState;
    };

    class BasicShootComponent : public ShootComponent {
    public:
        using ShootComponent::ShootComponent;

        void tryShoot() noexcept override;
    };

    class TripleShootComponent : public ShootComponent {
    public:
        using ShootComponent::ShootComponent;

        void tryShoot() noexcept override;

        sf::FloatRect getAffectedArea() const noexcept override;

        sf::FloatRect getStartShotBounds() const noexcept override;
    };

    class VolleyShootComponent : public ShootComponent {
    public:
        VolleyShootComponent(Airplane& owner, GameState& gameState) noexcept;

        void update(sf::Time elapsedTime) noexcept override;

        void tryShoot() noexcept override;
    private:
        int m_shots;
    };
}

#endif