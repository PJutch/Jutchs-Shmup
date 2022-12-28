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

#include "../GameState.h"

#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;

    class ShootControlComponent {
    public:
        virtual ~ShootControlComponent() = default;

        virtual void handleEvent(sf::Event event) noexcept {}

        virtual bool shouldShoot() noexcept = 0;
    };

    class AlwaysShootControlComponent : public ShootControlComponent {
    public:
        AlwaysShootControlComponent() noexcept = default;
        AlwaysShootControlComponent(Airplane&, GameState&) noexcept {}

        bool shouldShoot() noexcept override {
            return true;
        }
    };

    class NeverShootControlComponent : public ShootControlComponent {
    public:
        NeverShootControlComponent() noexcept = default;
        NeverShootControlComponent(Airplane&, GameState&) noexcept {}

        bool shouldShoot() noexcept override {
            return false;
        }
    };

    namespace detail {
        template <std::derived_from<ShootControlComponent> Component>
        class NotShootControlComponent : public ShootControlComponent {
        public:
            explicit NotShootControlComponent(Component component) noexcept : 
                m_component{std::move(component)} {}
            
            bool shouldShoot() noexcept override {
                return !m_component.shouldShoot();
            }
        private:
            Component m_component;
        };
    }

    template <typename Component>
        requires std::derived_from<std::remove_cvref_t<Component>, ShootControlComponent>
    inline auto operator ! (Component&& component) noexcept {
        using ResComponent = detail::NotShootControlComponent<std::remove_cvref_t<Component>>;
        return ResComponent{std::forward<Component>(component)};
    }

    namespace detail {
        template <std::derived_from<ShootControlComponent> Component1, 
                  std::derived_from<ShootControlComponent> Component2>
        class BinaryShootControlComponent : public ShootControlComponent {
        public:
            BinaryShootControlComponent(Component1 component1, Component2 component2) noexcept :
                m_component1{std::move(component1)}, m_component2{std::move(component2)} {}
        protected:
            bool shouldShoot1() noexcept {
                return m_component1.shouldShoot();
            }

            bool shouldShoot2() noexcept {
                return m_component2.shouldShoot();
            }
        private:
            Component1 m_component1;
            Component2 m_component2;
        };
    }
    namespace detail {
        template <std::derived_from<ShootControlComponent> Component1, 
                  std::derived_from<ShootControlComponent> Component2>
        class OrShootControlComponent : public BinaryShootControlComponent<Component1, Component2> {
            using Base = BinaryShootControlComponent<Component1, Component2>;
        public:
            using Base::BinaryShootControlComponent;
            
            bool shouldShoot() noexcept override {
                return Base::shouldShoot1() || Base::shouldShoot2();
            }
        };
    }

    template <typename Component1, typename Component2> 
        requires std::derived_from<std::remove_cvref_t<Component1>, ShootControlComponent>
              && std::derived_from<std::remove_cvref_t<Component2>, ShootControlComponent>
    inline auto operator || (Component1&& component1, Component2&& component2) noexcept {
        using ResComponent = detail::OrShootControlComponent<std::remove_cvref_t<Component1>, 
                                                             std::remove_cvref_t<Component2>>;
        return ResComponent{std::forward<Component1>(component1), 
                            std::forward<Component2>(component2)};
    }

    namespace detail {
        template <std::derived_from<ShootControlComponent> Component1, 
                  std::derived_from<ShootControlComponent> Component2>
        class AndShootControlComponent : public BinaryShootControlComponent<Component1, Component2> {
            using Base = BinaryShootControlComponent<Component1, Component2>;
        public:
            using Base::BinaryShootControlComponent;
            
            bool shouldShoot() noexcept override {
                return Base::shouldShoot1() && Base::shouldShoot2();
            }
        };
    }

    template <typename Component1, typename Component2> 
        requires std::derived_from<std::remove_cvref_t<Component1>, ShootControlComponent>
              && std::derived_from<std::remove_cvref_t<Component2>, ShootControlComponent>
    inline auto operator && (Component1&& component1, Component2&& component2) noexcept {
        using ResComponent = detail::AndShootControlComponent<std::remove_cvref_t<Component1>, 
                                                              std::remove_cvref_t<Component2>>;
        return ResComponent{std::forward<Component1>(component1), 
                            std::forward<Component2>(component2)};
    }

    class PlayerShootControlComponent : public ShootControlComponent {
    public:
        PlayerShootControlComponent() noexcept : m_shouldShoot{false} {}
        PlayerShootControlComponent(Airplane&, GameState&) noexcept : PlayerShootControlComponent{} {}

        void handleEvent(sf::Event event) noexcept override{
            if (event.type == sf::Event::MouseButtonPressed 
                && event.mouseButton.button == sf::Mouse::Left) m_shouldShoot = true;
        }

        bool shouldShoot() noexcept override  {
            bool shoot = sf::Mouse::isButtonPressed(sf::Mouse::Left) || m_shouldShoot;
            m_shouldShoot = false;
            return shoot;
        }
    private:
        bool m_shouldShoot;
    };

    class TargetPlayerShootControlComponent : public ShootControlComponent {
    public:        
        TargetPlayerShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        bool shouldShoot() noexcept override;
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };

    class CanHitPlayerShootControlComponent : public ShootControlComponent {
    public:        
        CanHitPlayerShootControlComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        bool shouldShoot() noexcept override;
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };
}

#endif