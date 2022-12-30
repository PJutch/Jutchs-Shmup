/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "MoveComponents.h"

#include "../GameState.h"

#include "../geometry.h"
#include "../algorithm.h"

#include <algorithm>

namespace Airplane {
    namespace {
        std::tuple<float, float> getMinmaxYFor(Airplane& airplane, GameState& gameState) {
            auto airplaneBounds = airplane.getGlobalBounds();

            auto obstacles = gameState.getEntities().getObstaclesFor(airplane) 
                | std::views::filter([airplaneBounds](sf::FloatRect obstacle) {
                    return intersects(left(airplaneBounds), right(airplaneBounds), 
                                      left(obstacle),       right(obstacle));
                });

            float minTop = max_value(obstacles 
                    | std::views::transform(bottom<float>) 
                    | std::views::filter(less_than(top(airplaneBounds))), 
                -gameState.getGameHeight() / 2);

            float maxBottom = min_value(obstacles 
                    | std::views::transform(top<float>) 
                    | std::views::filter(greater_than(bottom(airplaneBounds))), 
                gameState.getGameHeight() / 2);

            float posOffset = airplane.getPosition().y - airplaneBounds.top;
            return std::tuple<float, float>{minTop + posOffset, 
                                            maxBottom - airplaneBounds.height + posOffset};
        }
    }

    void PeriodicalMoveComponent::update(sf::Time elapsedTime) {
        auto moved = m_speed * elapsedTime.asSeconds();

        auto [minY, maxY] = getMinmaxYFor(m_owner, m_gameState); 

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

    void LineWithTargetMoveComponent::update(sf::Time elapsedTime) {
        auto moved = m_speed * elapsedTime.asSeconds();
        auto target = m_getTarget(m_owner);

        auto [minY, maxY] = getMinmaxYFor(m_owner, m_gameState);  
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

    void PlayerMoveComponent::update(sf::Time elapsedTime) {
        auto [movedX, movedY] = getMoved(elapsedTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movedX *= 2;
        m_owner.move(movedX, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            m_owner.move(0.f, -movedY);

            auto globalBounds = m_owner.getGlobalBounds();
            if (top(globalBounds)    < -m_gameState.getGameHeight() / 2)
                m_owner.setY(-m_gameState.getGameHeight() / 2 + globalBounds.height / 2.f);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            m_owner.move(0.f,  movedY);

            auto globalBounds = m_owner.getGlobalBounds();
            if (bottom(globalBounds) >  m_gameState.getGameHeight() / 2)
                m_owner.setY( m_gameState.getGameHeight() / 2 - globalBounds.height / 2.f);
        }
    }
}
