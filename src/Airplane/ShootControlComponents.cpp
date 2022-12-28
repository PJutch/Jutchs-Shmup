/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootControlComponents.h"

#include "../geometry.h"

#include <algorithm>

namespace Airplane {
    bool TargetPlayerShootControlComponent::shouldShoot() noexcept {
        return intersects(m_gameState.getEntities().getPlayerGlobalBounds(),
                          m_owner.getShootGlobalAffectedArea());
    }

    bool CanHitPlayerShootControlComponent::shouldShoot() noexcept {
        sf::FloatRect playerBounds = m_gameState.getEntities().getPlayerGlobalBounds();
        sf::FloatRect ownerBounds = m_owner.getGlobalBounds();

        if (right(playerBounds) >= left(ownerBounds)) return false;

        return std::ranges::none_of(m_gameState.getEntities().getObstaclesFor(m_owner), 
            [ownerBounds, playerBounds](sf::FloatRect globalBounds) {
                return intersects(top(globalBounds), bottom(globalBounds),
                                  top(ownerBounds),  bottom(ownerBounds))
                    && between(right(globalBounds), right(playerBounds), left(ownerBounds));
            });
    }
}
