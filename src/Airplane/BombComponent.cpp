/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Airplane/BombComponent.h"

#include "Airplane/Airplane.h"

namespace Airplane {
    BombComponent::BombComponent(Airplane& owner, GameState& gameState) noexcept :
        m_owner{owner}, m_gameState{gameState} {}

    void BombComponent::tryBomb() {
        if (m_owner.tryRemoveBomb())
            m_gameState.getEntities().addEntity(new Bomb{m_owner.getPosition(), m_gameState});
    }

    void EnemyBombComponent::update(sf::Time elapsedTime) {
        auto position = m_owner.getPosition();
        const auto& land = m_gameState.getLand();
        if (land.isXValid(position.x) && isEnemyTarget(land[position]))
            tryBomb();
    }
}
