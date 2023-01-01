/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_MOVE_COMPONENTS_H_
#define AIRPLANE_MOVE_COMPONENTS_H_

#include "MoveComponent.h"

#include "Airplane.h"
#include "../GameState.h"

#include <functional>

namespace Airplane {
    namespace detail {
        std::tuple<float, float> getMinmaxYFor(Airplane& airplane, GameState& gameState);
    }

    class BasicMoveComponent : public MoveComponent {
    public:
        BasicMoveComponent(Airplane& owner) : m_owner{owner} {}

        void update(sf::Time elapsedTime) noexcept override {
            auto moved = m_speed * elapsedTime.asSeconds();
            m_owner.move(-moved.x, 0.f);
        }

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    private:
        Airplane& m_owner;
    };

    class PeriodicalMoveComponent : public MoveComponent {
    public:
        PeriodicalMoveComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_moveUp{true}, m_owner{owner}, m_gameState{gameState} {}

        void update(sf::Time elapsedTime) override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    private:
        bool m_moveUp;
        Airplane& m_owner;
        GameState& m_gameState;
    };

    template <typename TargetGetter> 
        requires std::convertible_to<std::invoke_result_t<TargetGetter, const Airplane&>, sf::Vector2f>
    class LineWithTargetMoveComponent : public MoveComponent {
    public:
        LineWithTargetMoveComponent(Airplane& owner, GameState& gameState, 
                                    TargetGetter getTarget) noexcept : 
            m_getTarget{std::move(getTarget)}, m_owner{owner}, m_gameState{gameState} {}

        void update(sf::Time elapsedTime) override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    private:
        TargetGetter m_getTarget;
        Airplane& m_owner;
        GameState& m_gameState;
    };

    const auto createLineWithTargetMoveComponent = 
            []<typename TargetGetter>(Airplane& owner, GameState& gameState, TargetGetter&& getTarget) 
            requires std::convertible_to<std::invoke_result_t<TargetGetter, const Airplane&>, 
                                         sf::Vector2f> {
        using Component = LineWithTargetMoveComponent<TargetGetter>;
        return std::make_unique<Component>(owner, gameState, std::forward<TargetGetter>(getTarget));
    };

    template <typename TargetGetter>
        requires std::convertible_to<std::invoke_result_t<TargetGetter, const Airplane&>, sf::Vector2f>
    void LineWithTargetMoveComponent<TargetGetter>::update(sf::Time elapsedTime) {
        auto moved = m_speed * elapsedTime.asSeconds();
        auto target = m_getTarget(m_owner);

        auto [minY, maxY] = detail::getMinmaxYFor(m_owner, m_gameState);  
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

    class PlayerMoveComponent : public MoveComponent {
    public:
        PlayerMoveComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        void update(sf::Time elapsedTime) override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {m_speed.x, 0};
        }
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };
}

#endif