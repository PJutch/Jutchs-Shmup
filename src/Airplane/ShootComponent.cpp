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
    void ShootComponent::spawnBullet(sf::Vector2f offset) noexcept {
        m_gameState.getEntities().addEntity<Bullet>(m_owner.isOnPlayerSide(), 
                                                    m_owner.getPosition() + offset);
    }

    sf::FloatRect ShootComponent::getLocalAffectedArea() const noexcept {
        auto size = Bullet::getSize(m_gameState);
        return {-size.x / 2.f, -size.y / 2.f,  INFINITY, size.y};
    }

    sf::FloatRect ShootComponent::getGlobalAffectedArea() const noexcept {
        auto [top, left, width, height] = getLocalAffectedArea();
        auto [x, y] = m_owner.getPosition();

        if (m_owner.isOnPlayerSide())
            return {left + x, top + y,  width,  height};
        else
            return {left - x, top - y, -width, -height};
    }
}
