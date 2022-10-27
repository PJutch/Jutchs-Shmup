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

#include "SFML/System.hpp"
using sf::Time;
using sf::Event;
using sf::Mouse;

ShootControlComponent::ShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
    m_owner{owner}, m_gameState{gameState} {}

void BasicShootControlComponent::update(Time elapsedTime) noexcept {
    m_owner.getShootComponent().tryShoot();
}

void TargetPlayerShootControlComponent::update(Time elapsedTime) noexcept {
    auto& shootComponent = m_owner.getShootComponent();
    if (m_gameState.getPlayer().getGlobalBounds().intersects(shootComponent.getAffectedArea())) {
        shootComponent.tryShoot();
    }
}

void PlayerShootControlComponent::handleEvent(Event event) noexcept {
    if (event.type == Event::MouseButtonPressed 
        && event.mouseButton.button == Mouse::Left) m_owner.getShootComponent().tryShoot();
}

void PlayerShootControlComponent::update(Time elapsedTime) noexcept {
    if (Mouse::isButtonPressed(Mouse::Left)) m_owner.getShootComponent().tryShoot();
}
