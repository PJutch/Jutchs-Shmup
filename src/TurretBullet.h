/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef TURRET_BULLET_H_
#define TURRET_BULLET_H_

#include "Sprite.h"
#include "Entity.h"

#include <SFML/Graphics.hpp>

class TurretBullet : public Sprite, public CollidableBase<TurretBullet> {
public:
    TurretBullet(GameState& gameState, 
        sf::Vector2f position, sf::Vector2f direction) noexcept;
    
    void update(sf::Time elapsedTime) noexcept override {
        move(m_speed * elapsedTime.asSeconds());
    }

    bool shouldBeDeleted() const noexcept override;
private:
    sf::Vector2f m_speed;
    sf::Time m_launched;
    bool m_alive;
};

#endif