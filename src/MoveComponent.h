/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef MOVE_COMPONENT_H_
#define MOVE_COMPONENT_H_

#include "GameState.h"

#include <SFML/System.hpp>

class Airplane;

class MoveComponent {
public:
    MoveComponent(Airplane& owner, GameState& gameState);
    virtual ~MoveComponent() = default;

    virtual void update(sf::Time elapsedTime) noexcept = 0;

    virtual float getRotation() const noexcept = 0;
protected:
    Airplane& m_owner;
    GameState& m_gameState;
};

class BasicMoveComponent : public MoveComponent {
public:
    using MoveComponent::MoveComponent;

    void update(sf::Time elapsedTime) noexcept override;

    virtual float getRotation() const noexcept {
        return -90.f;
    }
};

class PlayerMoveComponent : public MoveComponent {
public:
    using MoveComponent::MoveComponent;

    void update(sf::Time elapsedTime) noexcept override;

    virtual float getRotation() const noexcept {
        return 90.f;
    }
};

#endif