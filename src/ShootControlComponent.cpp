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

ShootControlComponent::ShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
    m_shootComponent{nullptr}, m_owner{owner}, m_gameState{gameState} {}

void TargetPlayerShootControlComponent::update(sf::Time elapsedTime) noexcept {
    if (m_gameState.getPlayer().getGlobalBounds().intersects(m_shootComponent->getAffectedArea())) {
        m_shootComponent->tryShoot();
    }
}
