/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "DeathComponent.h"

#include "HealthPickup.h"

DeathComponent::DeathComponent(Airplane& owner, GameState& gameState) : 
        m_owner{owner}, m_gameState{gameState} {}

void LootDeathComponent::handleDeath() noexcept {
    m_gameState.addScore(10);
    if (m_gameState.genRandom(std::uniform_real_distribution{0.0, 1.0}) < 0.1) {
        m_gameState.addEntity(new HealthPickup{m_owner.getPosition(), m_gameState});
    }
}
