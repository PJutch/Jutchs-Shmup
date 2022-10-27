/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SHOOT_CONTROL_COMPONENT_H_
#define SHOOT_CONTROL_COMPONENT_H_

#include "ShootComponent.h"
#include "GameState.h"

#include <SFML/System.hpp>

class Airplane;

class ShootControlComponent {
public:
    ShootControlComponent(Airplane& owner, GameState& gameState) noexcept;
    virtual ~ShootControlComponent() = default;

    virtual void handleEvent(sf::Event event) noexcept {}

    virtual void update(sf::Time elapsedTime) noexcept = 0;
protected:
    Airplane& m_owner;
    GameState& m_gameState;
};

class BasicShootControlComponent : public ShootControlComponent {
public:
    using ShootControlComponent::ShootControlComponent;

    void update(sf::Time elapsedTime) noexcept override;
};

class TargetPlayerShootControlComponent : public ShootControlComponent {
public:
    using ShootControlComponent::ShootControlComponent;

    void update(sf::Time elapsedTime) noexcept override;
};

class PlayerShootControlComponent : public ShootControlComponent {
public:
    using ShootControlComponent::ShootControlComponent;

    void handleEvent(sf::Event event) noexcept override;
    void update(sf::Time elapsedTime) noexcept override;
};

#endif