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

#include "GameState.h"
#include "Entity.h"
#include "Bullet.h"
#include "Pickup.h"
#include "Airplane/Airplane.h"
#include "Airplane/ShootComponent.h"
#include "Airplane/ShootControlComponent.h"
#include "Airplane/MoveComponent.h"
#include "Airplane/DeathEffect.h"

namespace Airplane {
    class Builder {
    public:
        template<typename... Args>
        Builder(GameState& gameState, Args&&... args);

        Builder& maxHealth(int maxHealth) noexcept {
            m_build->m_maxHealth = maxHealth;
            m_build->m_health = maxHealth;
            return *this;
        }

        Builder& position(sf::Vector2f position) noexcept {
            m_build->m_sprite.setPosition(position);
            return *this;
        }

        Builder& deletable(bool deletable) noexcept {
            m_build->m_deletable = deletable;
            return *this;
        }

        Builder& canUsePickups(bool canUsePickups) noexcept {
            m_build->m_canUsePickups = canUsePickups;
            return *this;
        }

        Builder& speed(sf::Vector2f speed) noexcept {
            m_speed = speed;
            return *this;
        }

        Builder& playerSide(bool playerSide) noexcept {
            m_build->m_playerSide = playerSide;
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

        template<std::derived_from<DeathEffect> Component, typename... Args>
        Builder& addDeathEffect(Args&&... args) {
            m_build->m_deathEffects.emplace_back(
                std::make_unique<Component>(*m_build, m_gameState, std::forward<Args>(args)...));
            return *this;
        }

        Builder& textureHeavy(bool heavy) noexcept {
            m_textureHeavy = heavy;
            return *this;
        }

        Builder& textureFast(bool fast) noexcept {
            m_textureFast = fast;
            return *this;
        }

        Builder& textureHasWeapon(bool hasWeapon) noexcept {
            m_textureHasWeapon = hasWeapon;
            return *this;
        }

        std::unique_ptr<Airplane> build() noexcept {
            m_build->m_moveComponent->setSpeed(m_speed);
            
            setTexture(m_gameState.getAssets().getAirplaneTexture(m_build->m_playerSide, 
                                                                m_textureHeavy, 
                                                                m_textureFast, 
                                                                m_textureHasWeapon));
            
            return std::move(m_build);
        }
    private:
        std::unique_ptr<Airplane> m_build;

        sf::Vector2f m_speed;

        bool m_textureHeavy;
        bool m_textureFast;
        bool m_textureHasWeapon;

        GameState& m_gameState;

        void setTexture(const sf::Texture& texture) noexcept {
            m_build->m_sprite.setTexture(texture);

            auto size = texture.getSize();
            m_build->m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
        }
    };

    template<typename... Args>
    Builder::Builder(GameState& gameState, Args&&... args) : 
        m_build{new Airplane{gameState, std::forward<Args>(args)...}}, m_gameState{gameState},         
        m_speed{0.f, 0.f}, m_textureHeavy{false}, m_textureFast{false}, m_textureHasWeapon{false} {}
}

#endif
