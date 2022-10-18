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

#include <SFML/System.hpp>

class ShootControlComponent {
public:
    ShootControlComponent(ShootComponent& shootComponent) : m_shootComponent{shootComponent} {}

    virtual ~ShootControlComponent() = default;

    virtual void handleMouseButtonPressed(sf::Event::MouseButtonEvent event) noexcept {}

    virtual void update(sf::Time elapsedTime) noexcept = 0;
protected:
    ShootComponent& m_shootComponent;
};

class BasicShootControlComponent : public ShootControlComponent {
public:
    using ShootControlComponent::ShootControlComponent;

    void update(sf::Time elapsedTime) noexcept override {
        m_shootComponent.tryShoot();
    }
};

class PlayerShootControlComponent : public ShootControlComponent {
public:
    using ShootControlComponent::ShootControlComponent;

    void handleMouseButtonPressed(sf::Event::MouseButtonEvent event) noexcept override {
        if (event.button == sf::Mouse::Left) m_shootComponent.tryShoot();
    }

    void update(sf::Time elapsedTime) noexcept override {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) m_shootComponent.tryShoot();
    }
};

#endif