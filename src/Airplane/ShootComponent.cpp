/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootComponent.h"

#include "Airplane.h"

#include "../SoundEffect.h"
#include "../Bullet.h"

#include <cmath>

namespace Airplane {
    void ShootComponent::shoot(sf::Vector2f position) noexcept {
        m_gameState.getEntities().addEntity<Bullet>(m_owner.isOnPlayerSide(), position);
    }

    sf::FloatRect ShootComponent::getAffectedArea() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        if (m_owner.isOnPlayerSide()) {
            return {position.x - size.x / 2.f, position.y - size.y / 2.f, INFINITY, size.y};
        } else {
            return {position.x + size.x / 2.f, position.y - size.y / 2.f, -INFINITY, size.y};
        }
    }   

    sf::FloatRect ShootComponent::getStartShotBounds() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        return {position.x - size.x / 2.f, position.y - size.y / 2.f, size.x, size.y};
    }   

    sf::Vector2f ShootComponent::getShotSpeed() const noexcept {
        auto speed = Bullet::getSpeed();
        return {(m_owner.isOnPlayerSide() ? 1 : -1) * speed.x, speed.y};
    }
}
