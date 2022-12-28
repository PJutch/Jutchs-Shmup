/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef BOMB_COMPONENTS_H_
#define BOMB_COMPONENTS_H_

#include "BombComponent.h"

#include "Airplane.h"
#include "../GameState.h"

namespace Airplane {
    class EnemyBombComponent : public BombComponent {
    public:
        using BombComponent::BombComponent;

        void update(sf::Time elapsedTime) override {
            auto position = m_owner.getPosition();
            const auto& land = m_gameState.getLand();
            if (land.isPosValid(position) && isEnemyTarget(land[position]))
                tryBomb();
        }
    };

    class PlayerBombComponent : public BombComponent {
    public:
        using BombComponent::BombComponent;

        void handleEvent(sf::Event event) override {
            if (event.type == sf::Event::MouseButtonPressed 
             && event.mouseButton.button == sf::Mouse::Right) {
                tryBomb();
            }
        }
    };
}

#endif