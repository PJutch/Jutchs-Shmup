/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_MOVE_COMPONENT_H_
#define AIRPLANE_MOVE_COMPONENT_H_

#include "GameState.h"

#include <SFML/System.hpp>

#include <tuple>

namespace Airplane {
    class Airplane;

    class MoveComponent {
    public:
        MoveComponent(Airplane& owner, GameState& gameState) noexcept;
        virtual ~MoveComponent() = default;

        void setSpeed(sf::Vector2f speed) noexcept {
            m_speed = speed;
        }

        virtual void update(sf::Time elapsedTime) noexcept = 0;

        virtual sf::Vector2f getMinSpeed() const noexcept = 0;
    protected:
        Airplane& m_owner;
        GameState& m_gameState;
        sf::Vector2f m_speed;

        std::tuple<float, float> getMinmaxY() const noexcept;
    };

    class BasicMoveComponent : public MoveComponent {
    public:
        using MoveComponent::MoveComponent;

        void update(sf::Time elapsedTime) noexcept override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    };

    class PeriodicalMoveComponent : public MoveComponent {
    public:
        PeriodicalMoveComponent(Airplane& owner, GameState& gameState) noexcept;

        void update(sf::Time elapsedTime) noexcept override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    private:
        bool m_moveUp;
    };

    class FollowPlayerMoveComponent : public MoveComponent {
    public:
        using MoveComponent::MoveComponent;

        void update(sf::Time elapsedTime) noexcept override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {-m_speed.x, 0};
        }
    private:
        bool m_moveUp;
    };

    class PlayerMoveComponent : public MoveComponent {
    public:
        using MoveComponent::MoveComponent;

        void update(sf::Time elapsedTime) noexcept override;

        sf::Vector2f getMinSpeed() const noexcept override {
            return {m_speed.x, 0};
        }
    };
}

#endif