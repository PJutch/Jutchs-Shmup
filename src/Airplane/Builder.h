/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_BUILDER_H_
#define AIRPLANE_BUILDER_H_

#include "Airplane.h"

#include "../GameState.h"

#include "functional.h"

#include <concepts>
namespace Airplane {
    class Builder {
    public:
        template<typename... Args>
        explicit Builder(GameState& gameState, Args&&... args);

        Builder& maxHealth(int maxHealth) noexcept {
            m_build->m_healthComponent.setMaxHealth(maxHealth);
            return *this;
        }

        Builder& position(sf::Vector2f position) noexcept {
            m_build->setPosition(position);
            return *this;
        }

        Builder& position(float x, float y) noexcept {
            return position({x, y});
        }

        Builder& speed(sf::Vector2f speed) noexcept {
            m_speed = speed;
            return *this;
        }

        Builder& speed(float x, float y) noexcept {
            return speed({x, y});
        }

        Builder& shootPattern(ShootComponent::Pattern pattern) {
            m_build->m_shootComponent.setPattern(pattern);
            return *this;
        }

        template <typename Component, typename... Args> 
            requires std::constructible_from<Component, Airplane::Airplane&, GameState&, Args...>
        Component createComponent(Args&&... args) {
            return Component(*m_build, m_gameState, std::forward<Args>(args)...);
        }

        template <typename Component, typename... Args> 
            requires std::constructible_from<Component, GameState&, Args...>
        Component createComponent(Args&&... args) {
            return Component(m_gameState, std::forward<Args>(args)...);
        }

        template <typename Component, typename... Args> 
            requires std::constructible_from<Component, Airplane::Airplane&, Args...>
        Component createComponent(Args&&... args) {
            return Component(*m_build, std::forward<Args>(args)...);
        }

        template <typename Component, typename... Args> 
            requires std::constructible_from<Component, Args...>
        Component createComponent(Args&&... args) {
            return Component(std::forward<Args>(args)...);
        }

        template <typename Factory, typename... Args>
        Builder& shootControlComponent(Factory&& factory, Args&&... args) {
            m_build->m_shootControlComponent = newComponentBy<ShootControlComponent>(
                std::forward<Factory>(factory), std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        Builder& shootControlComponent(Args&&... args) {
            return shootControlComponent(makeUniqueFunctor<Component>, std::forward<Args>(args)...);
        }

        template <typename Component>
            requires std::derived_from<std::remove_cvref_t<Component>, ShootControlComponent>
        Builder& shootControlComponent(Component&& component) {
            using ComponentVal = std::remove_cvref_t<Component>;
            m_build->m_shootControlComponent 
                = std::make_unique<ComponentVal>(std::forward<Component>(component));
            return *this;
        }

        template <typename Factory, typename... Args>
        Builder& moveComponent(Factory&& factory, Args&&... args) {
            m_build->m_moveComponent = newComponentBy<MoveComponent>(std::forward<Factory>(factory), 
                                                                     std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<MoveComponent> Component, typename... Args>
        Builder& moveComponent(Args&&... args) {
            return moveComponent(makeUniqueFunctor<Component>, std::forward<Args>(args)...);
        }

        template <typename Component>
            requires std::derived_from<std::remove_cvref_t<Component>, MoveComponent>
        Builder& moveComponent(Component&& component) {
            using ComponentVal = std::remove_cvref_t<Component>;
            m_build->m_moveComponent
                = std::make_unique<ComponentVal>(std::forward<Component>(component));
            return *this;
        }

        template <typename Factory, typename... Args>
        Builder& bombComponent(Factory&& factory, Args&&... args) {
            m_build->m_bombComponent = newComponentBy<BombComponent>(std::forward<Factory>(factory), 
                                                                     std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<BombComponent> Component, typename... Args>
        Builder& bombComponent(Args&&... args) {
            return bombComponent(makeUniqueFunctor<Component>, std::forward<Args>(args)...);
        }

        template <typename Component>
            requires std::derived_from<std::remove_cvref_t<Component>, BombComponent>
        Builder& bombComponent(Component&& component) {
            using ComponentVal = std::remove_cvref_t<Component>;
            m_build->m_bombComponent
                = std::make_unique<ComponentVal>(std::forward<Component>(component));
            return *this;
        }

        Builder& bomb(bool hasBomb = true) noexcept {
            m_hasBomb = hasBomb;
            return *this;
        }

        template <typename Factory, typename... Args>
        Builder& addDeathEffect(Factory&& factory, Args&&... args) {
            m_build->m_deathEffects.push_back(newComponentBy<DeathEffect>(std::forward<Factory>(factory), 
                                                                          std::forward<Args>(args)...));
            return *this;
        }

        template<std::derived_from<DeathEffect> Effect, typename... Args>
        Builder& addDeathEffect(Args&&... args) {
            return addDeathEffect(makeUniqueFunctor<Effect>, std::forward<Args>(args)...);
        }

        template <typename Component>
            requires std::derived_from<std::remove_cvref_t<Component>, DeathEffect>
        Builder& addDeathEffect(Component&& component) {
            using ComponentVal = std::remove_cvref_t<Component>;
            m_build->m_deathEffects.push_back(
                std::make_unique<ComponentVal>(std::forward<Component>(component)));
            return *this;
        }

        Builder& flags(Flags flags) noexcept {
            m_build->m_flags = flags;
            return *this;
        }

        Flags& flags() noexcept {
            return m_build->m_flags;
        }

        std::unique_ptr<Airplane> build() noexcept {
            m_build->m_moveComponent->m_speed = m_speed;
            m_build->m_bombComponent->m_hasBomb = m_hasBomb;

            m_build->updateTexture();

            return std::move(m_build);
        }
    private:
        std::unique_ptr<Airplane> m_build;

        sf::Vector2f m_speed;
        bool m_hasBomb;

        GameState& m_gameState;

        template <typename Base, typename Factory, typename... Args> 
            requires std::convertible_to<
                std::invoke_result_t<Factory, Airplane::Airplane&, GameState&, Args...>, 
                std::unique_ptr<Base>>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            return std::invoke(std::forward<Factory>(factory), 
                               *m_build, m_gameState, std::forward<Args>(args)...);
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::derived_from<
                std::invoke_result_t<Factory, Airplane::Airplane&, GameState&, Args...>, Base>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            using Component = std::invoke_result_t<Factory, Airplane::Airplane&, GameState&, Args...>;
            return std::make_unique<Component>(
                std::invoke(std::forward<Factory>(factory), 
                            *m_build, m_gameState, std::forward<Args>(args)...));
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::convertible_to<
                std::invoke_result_t<Factory, GameState&, Args...>, 
                std::unique_ptr<Base>>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            return std::invoke(std::forward<Factory>(factory), m_gameState, std::forward<Args>(args)...);
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::derived_from<
                std::invoke_result_t<Factory, GameState&, Args...>, Base>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            using Component = std::invoke_result_t<Factory, GameState&, Args...>;
            return std::make_unique<Component>(
                std::invoke(std::forward<Factory>(factory), m_gameState, std::forward<Args>(args)...));
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::convertible_to<
                std::invoke_result_t<Factory, Airplane::Airplane&, Args...>, 
                std::unique_ptr<Base>>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            return std::invoke(std::forward<Factory>(factory), *m_build, std::forward<Args>(args)...);
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::derived_from<
                std::invoke_result_t<Factory, Airplane::Airplane&, Args...>, Base>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            using Component = std::invoke_result_t<Factory, Airplane::Airplane&, Args...>;
            return std::make_unique<Component>(
                std::invoke(std::forward<Factory>(factory), *m_build, std::forward<Args>(args)...));
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::convertible_to<
                std::invoke_result_t<Factory, Args...>, 
                std::unique_ptr<Base>>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            return std::invoke(std::forward<Factory>(factory), std::forward<Args>(args)...);
        }

        template <typename Base, typename Factory, typename... Args> 
            requires std::derived_from<
                std::invoke_result_t<Factory, Args...>, Base>
        std::unique_ptr<Base> newComponentBy(Factory&& factory, Args&&... args) {
            using Component = std::invoke_result_t<Factory, Args...>;
            return std::make_unique<Component>(
                std::invoke(std::forward<Factory>(factory), std::forward<Args>(args)...));
        }
    };

    template<typename... Args>
    Builder::Builder(GameState& gameState, Args&&... args) : 
        m_build{gameState.getEntities().createEntity<Airplane>(std::forward<Args>(args)...)}, 
        m_gameState{gameState}, m_speed{0.f, 0.f}, m_hasBomb{false} {}
}

#endif
