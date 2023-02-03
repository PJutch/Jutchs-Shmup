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
#include <cmath>

class TurretBullet : public Sprite, public CollidableBase<TurretBullet> {
public:
    TurretBullet(GameState& gameState, 
        sf::Vector2f position, sf::Vector2f direction) noexcept;
    
    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override;

    bool isAtMaxHeight() const noexcept {
        float t = (m_gameState.getCurrentTime() - m_launched).asSeconds();
        return t >= VERTICAL_SPEED / GRAVITY;
    }

    void acceptCollide(Airplane::Airplane& other) noexcept override;
private:
    sf::Vector2f m_speed;
    sf::Time m_launched;
    bool m_alive;

    static const constexpr float VERTICAL_SPEED = 4.472136f;
    static const constexpr float GRAVITY = 10.f;
};

#endif