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

namespace Airplane {
    class Builder {
    public:
        template<typename... Args>
        explicit Builder(GameState& gameState, Args&&... args);

        Builder& maxHealth(int maxHealth) noexcept {
            m_build->m_maxHealth = maxHealth;
            m_build->m_health = maxHealth;
            return *this;
        }

        Builder& position(sf::Vector2f position) noexcept {
            m_build->setPosition(position);
            return *this;
        }

        Builder& speed(sf::Vector2f speed) noexcept {
            m_speed = speed;
            return *this;
        }

        template<std::derived_from<ShootComponent> Component, typename... Args>
        Builder& shootComponent(Args&&... args) {
            m_build->m_shootComponent = std::make_unique<Component>
                (*m_build, m_gameState, std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        std::unique_ptr<ShootControlComponent> createShootControlComponent(Args&&... args) {
            return std::make_unique<Component>(*m_build, m_gameState, std::forward<Args>(args)...);
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        Builder& shootControlComponent(Args&&... args) {
            m_build->m_shootControlComponent
                = createShootControlComponent<Component>(std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<MoveComponent> Component, typename... Args>
        Builder& moveComponent(Args&&... args) {
            m_build->m_moveComponent = std::make_unique<Component>
                (*m_build, m_gameState, std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<BombComponent> Component, typename... Args>
        Builder& bombComponent(Args&&... args) {
            m_build->m_bombComponent = std::make_unique<Component>
                (*m_build, m_gameState, std::forward<Args>(args)...);
            return *this;
        }

        Builder& bomb(bool hasBomb = true) noexcept {
            m_hasBomb = hasBomb;
            return *this;
        }

        template<std::derived_from<DeathEffect> Component, typename... Args>
        Builder& addDeathEffect(Args&&... args) {
            m_build->m_deathEffects.emplace_back(
                std::make_unique<Component>(*m_build, m_gameState, std::forward<Args>(args)...));
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
            m_build->m_moveComponent->setSpeed(m_speed);
            m_build->m_bombComponent->setHasBomb(m_hasBomb);

            m_build->updateTexture();

            return std::move(m_build);
        }
    private:
        std::unique_ptr<Airplane> m_build;

        sf::Vector2f m_speed;
        bool m_hasBomb;

        GameState& m_gameState;
    };

    template<typename... Args>
    Builder::Builder(GameState& gameState, Args&&... args) : 
        m_build{new Airplane{gameState, std::forward<Args>(args)...}}, m_gameState{gameState},         
        m_speed{0.f, 0.f}, m_hasBomb{false} {}
}

#endif
