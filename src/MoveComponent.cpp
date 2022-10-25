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
using sf::Vector2f;

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::minmax;
using std::min;
using std::max;
using std::fmod;

MoveComponent::MoveComponent(Airplane& owner, GameState& gameState, Vector2f speed) noexcept : 
        m_owner{owner}, m_gameState{gameState}, m_speed{speed} {}

void BasicMoveComponent::update(Time elapsedTime) noexcept {
    float movedX = - m_speed.x * elapsedTime.asSeconds();
    m_owner.move({movedX, 0.f});
}

PeriodicalMoveComponent::PeriodicalMoveComponent(
        Airplane& owner, GameState& gameState, Vector2f speed) noexcept :
    MoveComponent(owner, gameState, speed), m_moveUp{true} {}

void PeriodicalMoveComponent::update(Time elapsedTime) noexcept {
    float movedX = - m_speed.x * elapsedTime.asSeconds();

    auto globalBounds = m_owner.getGlobalBounds();

    float maxY =  m_gameState.getGameHeight();
    float minY = -m_gameState.getGameHeight() + globalBounds.height;
    for (auto& entity : m_gameState.getEntities()) {
        if (entity->isPassable() || entity.get() == &m_owner) continue;

        auto globalBounds2 = entity->getGlobalBounds();
        if (globalBounds.left  <= globalBounds2.left + globalBounds2.width 
         && globalBounds2.left <= globalBounds.left  + globalBounds.width) {
            if (globalBounds2.top > globalBounds.top) {
                maxY = min(globalBounds2.top, maxY);
            } else {
                minY = max(globalBounds2.top, minY);
            }
        }
    }
    
    float y = m_owner.getPosition().y + (m_moveUp ? 1.f : -1.f) * m_speed.y * elapsedTime.asSeconds();
    if (y > maxY) {
        y = maxY - (y - maxY);
        m_moveUp = false;
    } else if (y < minY) {
        y = minY + (minY - y);
        m_moveUp = true;
    }
    
    m_owner.setPosition({m_owner.getPosition().x + movedX, y});
}

void FollowPlayerMoveComponent::update(Time elapsedTime) noexcept {
    float movedX = - m_speed.x * elapsedTime.asSeconds();

    auto globalBounds = m_owner.getGlobalBounds();

    float maxY =  m_gameState.getGameHeight();
    float minY = -m_gameState.getGameHeight() + globalBounds.height;
    for (auto& entity : m_gameState.getEntities()) {
        if (entity->isPassable() || entity.get() == &m_owner) continue;

        auto globalBounds2 = entity->getGlobalBounds();
        if (globalBounds.left  <= globalBounds2.left + globalBounds2.width 
         && globalBounds2.left <= globalBounds.left  + globalBounds.width) {
            if (globalBounds2.top > globalBounds.top) {
                maxY = min(globalBounds2.top, maxY);
            } else {
                minY = max(globalBounds2.top, minY);
            }
        }
    }
    
    float playerY = m_gameState.getPlayer().getPosition().y;
    float y = m_owner.getPosition().y;
    float movedY = m_speed.y * elapsedTime.asSeconds();
    if (abs(playerY - y) < movedY) {
        y = playerY;
    } else {
        y = y + (playerY > y ? 1.f : -1.f) * movedY;
        if (y > maxY) {
            y = maxY - (y - maxY);
            m_moveUp = false;
        } else if (y < minY) {
            y = minY + (minY - y);
            m_moveUp = true;
        }
    }
    
    m_owner.setPosition({m_owner.getPosition().x + movedX, y});
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
