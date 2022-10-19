/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "MoveComponent.h"

#include "Airplane.h"

#include <SFML/System.hpp>
using sf::Time;
using sf::Keyboard;

MoveComponent::MoveComponent(Airplane& owner, GameState& gameState, sf::Vector2f speed) noexcept : 
        m_owner{owner}, m_gameState{gameState}, m_speed{speed} {}

void BasicMoveComponent::update(Time elapsedTime) noexcept {
    float movedX = m_speed.x * elapsedTime.asSeconds();
    m_owner.move({-movedX, 0.f});
}

void PlayerMoveComponent::update(Time elapsedTime) noexcept {
    if (Keyboard::isKeyPressed(Keyboard::W)) {
        m_owner.move({0.f, -m_speed.y * elapsedTime.asSeconds()});

        auto globalBounds = m_owner.getGlobalBounds();
        if (globalBounds.top < -m_gameState.getGameHeight() / 2)
            m_owner.setPosition({m_owner.getPosition().x, 
                                 -m_gameState.getGameHeight() / 2 + globalBounds.height / 2.f});
    } else if (Keyboard::isKeyPressed(Keyboard::S)) {
        m_owner.move({0.f, m_speed.y * elapsedTime.asSeconds()});

        auto globalBounds = m_owner.getGlobalBounds();
        if (globalBounds.top + globalBounds.width > m_gameState.getGameHeight() / 2)
            m_owner.setPosition({m_owner.getPosition().x, 
                                 m_gameState.getGameHeight() / 2 - globalBounds.height / 2.f});
    }

    float movedX = m_speed.x * elapsedTime.asSeconds();
    if (Keyboard::isKeyPressed(Keyboard::D)) movedX *= 2;

    m_owner.move({movedX, 0.f});
}
