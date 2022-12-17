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

#include "Airplane/Airplane.h"

#include <SFML/System.hpp>
using sf::Time;
using sf::Keyboard;
using sf::Vector2f;

#include <iostream>
using std::cout;
using std::endl;

#include <tuple>
using std::tuple;

#include <algorithm>
using std::minmax;
using std::min;
using std::max;
using std::fmod;

namespace Airplane {
    MoveComponent::MoveComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState}, m_speed{Vector2f{0.f, 0.f}} {}

    tuple<float, float> MoveComponent::getMinmaxY() const noexcept {
        auto globalBounds = m_owner.getGlobalBounds();

        float minTop = -m_gameState.getGameHeight() / 2;
        float maxBottom =  m_gameState.getGameHeight() / 2;
        for (auto& entity : m_gameState.getEntities()) {
            if (entity->isPassable() || entity.get() == &m_owner) continue;

            auto globalBounds2 = entity->getGlobalBounds();
            if (globalBounds.left  <= globalBounds2.left + globalBounds2.width 
            && globalBounds2.left <= globalBounds.left  + globalBounds.width) {
                if (globalBounds2.top + globalBounds2.height < globalBounds.top) {
                    minTop = max(globalBounds2.top + globalBounds2.height, minTop);
                }

                if (globalBounds2.top > globalBounds.top + globalBounds.height) {
                    maxBottom = min(globalBounds2.top, maxBottom);
                }
            }
        }

        float posOffset = m_owner.getPosition().y - globalBounds.top;
        return tuple<float, float>{minTop + posOffset, maxBottom - globalBounds.height + posOffset};
    }

    void BasicMoveComponent::update(Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();
        m_owner.move(-moved.x, 0.f);
    }

    PeriodicalMoveComponent::PeriodicalMoveComponent(Airplane& owner, GameState& gameState) noexcept :
        MoveComponent(owner, gameState), m_moveUp{true} {}

    void PeriodicalMoveComponent::update(Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();

        auto [minY, maxY] = getMinmaxY();  
        float y = m_owner.getPosition().y + (m_moveUp ? 1.f : -1.f) * moved.y;
        if (y > maxY) {
            y = maxY - (y - maxY);
            m_moveUp = false;
        } else if (y < minY) {
            y = minY + (minY - y);
            m_moveUp = true;
        }
        
        m_owner.setPosition(m_owner.getPosition().x - moved.x, y);
    }

    LineWithTargetMoveComponent::LineWithTargetMoveComponent(
        Airplane& owner, GameState& gameState, TargetGetter getTarget) noexcept :
            MoveComponent(owner, gameState), m_getTarget{std::move(getTarget)}, m_moveUp{true} {}

    void LineWithTargetMoveComponent::update(Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();
        auto target = m_getTarget(m_owner);

        auto [minY, maxY] = getMinmaxY();  
        float targetY = target.y;
        float y = m_owner.getPosition().y;
        if (abs(targetY - y) < moved.y) {
            y = targetY;
        } else {
            y = y + (targetY > y ? 1.f : -1.f) * moved.y;
            if (y > maxY) {
                y = maxY;
            } else if (y < minY) {
                y = minY;
            }
        }
        
        m_owner.setPosition(m_owner.getPosition().x - moved.x, y);
    }

    void PlayerMoveComponent::update(Time elapsedTime) noexcept {
        if (Keyboard::isKeyPressed(Keyboard::W)) {
            m_owner.move(0.f, -m_speed.y * elapsedTime.asSeconds());

            auto globalBounds = m_owner.getGlobalBounds();
            if (globalBounds.top < -m_gameState.getGameHeight() / 2)
                m_owner.setPosition(m_owner.getPosition().x, 
                                    -m_gameState.getGameHeight() / 2 + globalBounds.height / 2.f);
        } else if (Keyboard::isKeyPressed(Keyboard::S)) {
            m_owner.move(0.f, m_speed.y * elapsedTime.asSeconds());

            auto globalBounds = m_owner.getGlobalBounds();
            if (globalBounds.top + globalBounds.width > m_gameState.getGameHeight() / 2)
                m_owner.setPosition(m_owner.getPosition().x, 
                                    m_gameState.getGameHeight() / 2 - globalBounds.height / 2.f);
        }

        float movedX = m_speed.x * elapsedTime.asSeconds();
        if (Keyboard::isKeyPressed(Keyboard::D)) movedX *= 2;

        m_owner.move(movedX, 0.f);
    }
}
