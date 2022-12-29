/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_SHOOT_CONTROL_COMPONENTS_H_
#define AIRPLANE_SHOOT_CONTROL_COMPONENTS_H_

#include "ShootControlComponent.h"
#include "Airplane.h"

#include "../GameState.h"

#include "../geometry.h"

namespace Airplane {
    class PlayerShootControlComponent : public ShootControlComponent {
    public:
        PlayerShootControlComponent() noexcept : m_shouldShoot{false} {}

        void handleEvent(sf::Event event) noexcept override{
            if (event.type == sf::Event::MouseButtonPressed 
                && event.mouseButton.button == sf::Mouse::Left) m_shouldShoot = true;
        }

        bool shouldShoot() noexcept override  {
            bool shoot = sf::Mouse::isButtonPressed(sf::Mouse::Left) || m_shouldShoot;
            m_shouldShoot = false;
            return shoot;
        }
    private:
        bool m_shouldShoot;
    };

    class TargetPlayerShootControlComponent : public ShootControlComponent {
    public:        
        TargetPlayerShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        bool shouldShoot() noexcept override {
            return intersects(m_gameState.getEntities().getPlayerGlobalBounds(),
                            m_owner.getShootGlobalAffectedArea());
        }
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };

    class CanHitPlayerShootControlComponent : public ShootControlComponent {
    public:        
        CanHitPlayerShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        bool shouldShoot() override;
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };
}

#endif