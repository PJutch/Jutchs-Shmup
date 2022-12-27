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

#include "../geometry.h"

#include <algorithm>
#include <memory>

namespace Airplane {
    bool TargetPlayerShootControlComponent::shouldShoot() noexcept {
        return intersects(m_gameState.getEntities().getPlayerGlobalBounds(),
                          m_owner.getShootComponent().getAffectedArea());
    }

    bool CanHitPlayerShootControlComponent::shouldShoot() noexcept {
        sf::FloatRect playerBounds = m_gameState.getEntities().getPlayerGlobalBounds();
        sf::FloatRect ownerBounds = m_owner.getGlobalBounds();

        if (right(playerBounds) >= left(ownerBounds)) return false;

        return std::ranges::none_of(m_gameState.getEntities() 
               | std::views::filter([this](const std::unique_ptr<Entity>& entity) {
                return entity.get() != &m_owner;
            }) | std::views::filter([](const std::unique_ptr<Entity>& entity) {
                return !entity->isPassable();
            }) | std::views::transform([](const std::unique_ptr<Entity>& entity) {
                return entity->getGlobalBounds();
            }) | std::views::filter([ownerBounds](sf::FloatRect globalBounds) {
                return intersects(top(globalBounds), bottom(globalBounds),
                                  top(ownerBounds),  bottom(ownerBounds));
            }), [ownerBounds, playerBounds](sf::FloatRect globalBounds) {
                return between(right(globalBounds), right(playerBounds), left(ownerBounds));
            });
    }
}
