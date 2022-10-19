/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef DEATH_COMPONENT_H_
#define DEATH_COMPONENT_H_

#include "GameState.h"

#include <SFML/System.hpp>

class Airplane;

// Death effect
class DeathComponent {
public:
    DeathComponent(Airplane& owner, GameState& gameState);
    virtual ~DeathComponent() = default;

    virtual void handleDeath() noexcept = 0;
protected:
    Airplane& m_owner;
    GameState& m_gameState;
};

class PlayerDeathComponent : public DeathComponent {
public:
    using DeathComponent::DeathComponent;

    void handleDeath() noexcept override {
        m_gameState.setShouldReset();
    }
};

class LootDeathComponent : public DeathComponent {
public:
    using DeathComponent::DeathComponent;

    void handleDeath() noexcept override;
};

#endif