/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootControlComponent.h"

#include "Airplane.h"

#include "SFML/System.hpp"
using sf::Time;
using sf::Event;
using sf::Mouse;

#include <algorithm>
using std::max;
using std::min;

#include <memory>
using std::unique_ptr;

#include <utility>
using std::move;

namespace Airplane {
    bool TargetPlayerShootControlComponent::shouldShoot() noexcept {
        return m_gameState.getEntities().getPlayerGlobalBounds().intersects(
            m_owner.getShootComponent().getAffectedArea());
    }

    bool CanHitPlayerShootControlComponent::shouldShoot() noexcept {
        auto playerBounds = m_gameState.getEntities().getPlayerGlobalBounds();
        float playerRight = playerBounds.left + playerBounds.width;    

        auto ownerBounds = m_owner.getGlobalBounds();
        float ownerRight = ownerBounds.left + ownerBounds.width;

        if (playerRight < ownerBounds.left) {
            if (m_owner.isOnPlayerSide()) 
                return false;

            for (auto& entity : m_gameState.getEntities()) {
                if (entity->isPassable()) continue;
                
                auto globalBounds = entity->getGlobalBounds();
                float right = globalBounds.left + globalBounds.width;

                if (    globalBounds.top < ownerBounds.top + ownerBounds.height 
                    && globalBounds.top + globalBounds.height > ownerBounds.top
                    && right < ownerBounds.left && right > playerRight)
                    return false;
            }
        } else if (playerBounds.left > ownerRight) {
            if (!m_owner.isOnPlayerSide()) 
                return false;

            float minLeft = playerBounds.left;
            for (auto& entity : m_gameState.getEntities()) {
                if (!entity->isActive() || entity->isPassable()) continue;
                auto globalBounds = entity->getGlobalBounds();

                if (    globalBounds.top < ownerBounds.top + ownerBounds.height 
                    && globalBounds.top + globalBounds.height > ownerBounds.top
                    && globalBounds.left > ownerRight && globalBounds.left < playerBounds.left)
                    return false;
            }    
        }

        return true;
    }
}
