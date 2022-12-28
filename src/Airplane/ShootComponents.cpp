/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootComponents.h"

namespace Airplane {
    void TripleShootComponent::tryShoot() noexcept {
        if (m_shootCooldown <= sf::Time::Zero) {
            auto position = m_owner.getPosition();
            float height = m_owner.getGlobalBounds().height;

            shoot(position);
            shoot({position.x, position.y + height / 2});
            shoot({position.x, position.y - height / 2});

            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());

            m_shootCooldown = sf::seconds(0.5f);
        }
    }

    sf::FloatRect TripleShootComponent::getAffectedArea() const noexcept {
        auto position = m_owner.getPosition();
        float ownerHeight = m_owner.getGlobalBounds().height;
        auto size = Bullet::getSize(m_gameState);
        
        float areaHeight = ownerHeight + size.y;

        if (m_owner.isOnPlayerSide()) {
            return {position.x - size.x / 2.f, position.y - areaHeight / 2.f, INFINITY, areaHeight};
        } else {
            return {position.x + size.x / 2.f, position.y - areaHeight / 2.f, -INFINITY, areaHeight};
        }
    }

    sf::FloatRect TripleShootComponent::getStartShotBounds() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        float ownerHeight = m_owner.getGlobalBounds().height;   

        float areaHeight = ownerHeight + size.y;

        return {position.x - size.x / 2.f, position.y - areaHeight / 2.f, size.x, areaHeight};
    }  
}
