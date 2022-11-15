/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_SHOOT_CONTROL_COMPONENT_H_
#define AIRPLANE_SHOOT_CONTROL_COMPONENT_H_

#include "ShootComponent.h"
#include "GameState.h"

#include <SFML/System.hpp>

#include <memory>

namespace Airplane {
    class Airplane;

    class ShootControlComponent {
    public:
        ShootControlComponent(Airplane& owner, GameState& gameState) noexcept;
        virtual ~ShootControlComponent() = default;

        virtual void handleEvent(sf::Event event) noexcept {}
        virtual void update(sf::Time elapsedTime) noexcept {}

        virtual bool shouldShoot() noexcept = 0;
    protected:
        Airplane& m_owner;
        GameState& m_gameState;
    };

    class AlwaysShootControlComponent : public ShootControlComponent {
    public:
        using ShootControlComponent::ShootControlComponent;

        bool shouldShoot() noexcept override {
            return true;
        }
    };

    class NeverShootControlComponent : public ShootControlComponent {
    public:
        using ShootControlComponent::ShootControlComponent;

        bool shouldShoot() noexcept override {
            return false;
        }
    };

    class NotShootControlComponent : public ShootControlComponent {
    public:
        NotShootControlComponent(Airplane& owner, GameState& gameState, 
                                std::unique_ptr<ShootControlComponent>&& component) noexcept;
        
        bool shouldShoot() noexcept override {
            return !m_component->shouldShoot();
        }
    private:
        std::unique_ptr<ShootControlComponent> m_component;
    };

    class OrShootControlComponent : public ShootControlComponent {
    public:
        OrShootControlComponent(Airplane& owner, GameState& gameState, 
                                std::unique_ptr<ShootControlComponent>&& component1, 
                                std::unique_ptr<ShootControlComponent>&& component2) noexcept;
        
        bool shouldShoot() noexcept override {
            return m_component1->shouldShoot() || m_component2->shouldShoot();
        }
    private:
        std::unique_ptr<ShootControlComponent> m_component1;
        std::unique_ptr<ShootControlComponent> m_component2;
    };

    class AndShootControlComponent : public ShootControlComponent {
    public:
        AndShootControlComponent(Airplane& owner, GameState& gameState, 
                                std::unique_ptr<ShootControlComponent>&& component1, 
                                std::unique_ptr<ShootControlComponent>&& component2) noexcept;
        
        bool shouldShoot() noexcept override {
            return m_component1->shouldShoot() && m_component2->shouldShoot();
        }
    private:
        std::unique_ptr<ShootControlComponent> m_component1;
        std::unique_ptr<ShootControlComponent> m_component2;
    };

    class TargetPlayerShootControlComponent : public ShootControlComponent {
    public:
        using ShootControlComponent::ShootControlComponent;

        bool shouldShoot() noexcept override;
    };

    class CanHitPlayerShootControlComponent : public ShootControlComponent {
    public:
        using ShootControlComponent::ShootControlComponent;

        bool shouldShoot() noexcept override;
    };

    class PlayerShootControlComponent : public ShootControlComponent {
    public:
        PlayerShootControlComponent(Airplane& owner, GameState& gameState) noexcept;

        void handleEvent(sf::Event event) noexcept override;
        bool shouldShoot() noexcept override;
    private:
        bool m_shouldShoot;
    };
}

#endif