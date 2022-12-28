/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_SHOOT_COMPONENTS_H_
#define AIRPLANE_SHOOT_COMPONENTS_H_

#include "ShootComponent.h"

#include "Airplane.h"

#include "../GameState.h"
#include "Bullet.h"

namespace Airplane {
    class BasicShootComponent : public ShootComponent {
    public:
        using ShootComponent::ShootComponent;

        void shoot() noexcept override {
            spawnBullet();
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
            m_shootCooldown = sf::seconds(0.25f);
        }
    };

    class TripleShootComponent : public ShootComponent {
    public:
        using ShootComponent::ShootComponent;

        void shoot() noexcept override{
            float height = m_owner.getGlobalBounds().height;

            spawnBullet();
            spawnBullet(0.f,  height / 2);
            spawnBullet(0.f, -height / 2);

            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
            m_shootCooldown = sf::seconds(0.5f);
        }

        sf::FloatRect getLocalAffectedArea() const noexcept override {
            auto size = Bullet::getSize(m_gameState);
            
            float areaHeight = m_owner.getGlobalBounds().height + size.y;
            return {-size.x / 2.f, -areaHeight / 2.f, INFINITY, areaHeight};
        }
    };

    class VolleyShootComponent : public ShootComponent {
    public:
        VolleyShootComponent(Airplane& owner, GameState& gameState) noexcept : 
            ShootComponent{owner, gameState}, m_shots{0} {};

        void update(sf::Time elapsedTime) noexcept override {
            ShootComponent::update(elapsedTime);
            if (m_shootCooldown <= sf::Time::Zero && m_shots > 0) {
                spawnBullet();
                m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
                m_shootCooldown = sf::seconds(-- m_shots == 0 ? 0.5f : 0.1f);
            }
        }

        void shoot() noexcept override {
            spawnBullet();

            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());

            m_shots += 2;
            m_shootCooldown = sf::seconds(0.1f);
        }
    private:
        int m_shots;
    };
}

#endif