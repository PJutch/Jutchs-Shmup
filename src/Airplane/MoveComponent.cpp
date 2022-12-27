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

#include "../geometry.h"
#include "../algorithm.h"

#include <algorithm>
#include <memory>

namespace Airplane {
    MoveComponent::MoveComponent(Airplane& owner, GameState& gameState) noexcept : 
        m_owner{owner}, m_gameState{gameState}, m_speed{0.f, 0.f} {}

    std::tuple<float, float> MoveComponent::getMinmaxY() const noexcept {
        auto ownerBounds = m_owner.getGlobalBounds();

        auto obstacles = m_gameState.getEntities() 
           | std::views::filter([this](const std::unique_ptr<Entity>& entity) {
            return entity.get() != &m_owner;
        }) | std::views::filter([](const std::unique_ptr<Entity>& entity) {
            return !entity->isPassable();
        }) | std::views::transform([](const std::unique_ptr<Entity>& entity) {
            return entity->getGlobalBounds();
        }) | std::views::filter([ownerBounds](sf::FloatRect obstacleBounds) {
            return intersects(left(ownerBounds),    right(ownerBounds), 
                              left(obstacleBounds), right(obstacleBounds));
        });

        float minTop = max_value(obstacles 
           | std::views::filter([ownerBounds](sf::FloatRect obstacleBounds) {
            return bottom(obstacleBounds) < top(ownerBounds);
        }), -m_gameState.getGameHeight() / 2, {}, [](sf::FloatRect obstacleBounds) {
            return bottom(obstacleBounds);
        });

        float maxBottom = min_value(obstacles 
           | std::views::filter([ownerBounds](sf::FloatRect obstacleBounds) {
            return top(obstacleBounds) < bottom(ownerBounds);
        }), m_gameState.getGameHeight() / 2, {}, [](sf::FloatRect obstacleBounds) {
            return top(obstacleBounds);
        });

        float posOffset = m_owner.getPosition().y - ownerBounds.top;
        return std::tuple<float, float>{minTop + posOffset, 
                                        maxBottom - ownerBounds.height + posOffset};
    }

    void BasicMoveComponent::update(sf::Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();
        m_owner.move(-moved.x, 0.f);
    }

    PeriodicalMoveComponent::PeriodicalMoveComponent(Airplane& owner, GameState& gameState) noexcept :
        MoveComponent(owner, gameState), m_moveUp{true} {}

    void PeriodicalMoveComponent::update(sf::Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();

        auto [minY, maxY] = getMinmaxY(); 

        float deltaY = m_moveUp ? moved.y : -moved.y;
        if (maxY - minY < 2 * m_owner.getGlobalBounds().height) {
            deltaY = 0.f;
        }

        float y = m_owner.getPosition().y + deltaY;

        if (y > maxY) {
            y = m_owner.getPosition().y - deltaY;
            m_moveUp = false;
        } else if (y < minY) {
            y = m_owner.getPosition().y - deltaY;
            m_moveUp = true;
        }
        
        m_owner.setPosition(m_owner.getPosition().x - moved.x, y);
    }

    LineWithTargetMoveComponent::LineWithTargetMoveComponent(
        Airplane& owner, GameState& gameState, TargetGetter getTarget) noexcept :
            MoveComponent(owner, gameState), m_getTarget{std::move(getTarget)}, m_moveUp{true} {}

    void LineWithTargetMoveComponent::update(sf::Time elapsedTime) noexcept {
        auto moved = m_speed * elapsedTime.asSeconds();
        auto target = m_getTarget(m_owner);

        auto [minY, maxY] = getMinmaxY();  
        float targetY = target.y;
        float y = m_owner.getPosition().y;
        if (std::abs(targetY - y) < moved.y) {
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

    void PlayerMoveComponent::update(sf::Time elapsedTime) noexcept {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_owner.move(0.f, -m_speed.y * elapsedTime.asSeconds());

            auto globalBounds = m_owner.getGlobalBounds();
            if (globalBounds.top < -m_gameState.getGameHeight() / 2)
                m_owner.setPosition(m_owner.getPosition().x, 
                                    -m_gameState.getGameHeight() / 2 + globalBounds.height / 2.f);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_owner.move(0.f, m_speed.y * elapsedTime.asSeconds());

            auto globalBounds = m_owner.getGlobalBounds();
            if (globalBounds.top + globalBounds.width > m_gameState.getGameHeight() / 2)
                m_owner.setPosition(m_owner.getPosition().x, 
                                    m_gameState.getGameHeight() / 2 - globalBounds.height / 2.f);
        }

        float movedX = m_speed.x * elapsedTime.asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movedX *= 2;

        m_owner.move(movedX, 0.f);
    }
}
