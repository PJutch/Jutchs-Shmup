/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SHOOT_COMPONENT_H_
#define SHOOT_COMPONENT_H_

#include "Entity.h"
#include "Bullet.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <vector>
#include <memory>

class Airplane;
class Player;

class ShootComponent {
public:
    ShootComponent(Airplane& owner, float gameHeight, std::vector<std::unique_ptr<Entity>>& entities, 
                   const sf::Texture& bulletTexture, Player& player) noexcept;

    virtual ~ShootComponent() = default;

    virtual void update(sf::Time elapsedTime) noexcept {
        m_shootCooldown -= elapsedTime;
    }

    virtual void tryShoot(bool right) noexcept = 0;
protected:
    sf::Time m_shootCooldown;

    Airplane& m_owner;
    float m_gameHeight;
    std::vector<std::unique_ptr<Entity>>& m_entities;
    const sf::Texture& m_bulletTexture;
    Player& m_player;
};

class BasicShootComponent : public ShootComponent {
public:
    using ShootComponent::ShootComponent;

    void tryShoot(bool right) noexcept override;
};

class TripleShootComponent : public ShootComponent {
public:
    using ShootComponent::ShootComponent;

    void tryShoot(bool right) noexcept override;
};

#endif