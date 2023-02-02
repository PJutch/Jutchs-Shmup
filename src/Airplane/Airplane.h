/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_AIRPLANE_H_
#define AIRPLANE_AIRPLANE_H_

#include "ShootComponent.h"
#include "ShootControlComponent.h"
#include "MoveComponent.h"
#include "BombComponent.h"
#include "DeathEffect.h"
#include "HealthComponent.h"
#include "Flags.h"

#include "../GameState.h"
#include "../Sprite.h"
#include "../Bullet.h"
#include "../Bomb.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>
#include <algorithm>

namespace Airplane {
    class Airplane : public Sprite, public CollidableBase<Airplane> {
    public:
        // use Airplane::Builder instead
        Airplane(GameState& gameState) noexcept : 
            Sprite{gameState}, m_shootComponent{*this, gameState} {}

        void handleEvent(sf::Event event) noexcept override {
            m_shootControlComponent->handleEvent(event);
            m_bombComponent->handleEvent(event);
        }

        void acceptCollide(Airplane& other) noexcept override {
            damage();
        }

        void acceptCollide(Bullet& other) noexcept override {
            if (other.isOnPlayerSide() == isOnPlayerSide()) return;
            damage();
        }

        // return true on success
        bool tryHeal() noexcept { 
            return m_healthComponent.tryHeal();
        }

        int getHealth() const noexcept {
            return m_healthComponent.getHealth();
        }

        bool tryAddBomb() noexcept {
            return m_bombComponent->tryAddBomb();
        }

        sf::Vector2f getMinSpeed() const noexcept {
            return m_moveComponent->getMinSpeed();
        }

        sf::FloatRect getShootGlobalAffectedArea() const noexcept {
            return m_shootComponent.getGlobalAffectedArea();
        }

        void update(sf::Time elapsedTime) noexcept override {
            m_healthComponent.update(elapsedTime);
            m_shootComponent.update(elapsedTime);
            m_moveComponent ->update(elapsedTime);
            m_bombComponent ->update(elapsedTime);

            if (m_shootControlComponent->shouldShoot()) {
                m_shootComponent.trySetShouldShoot();
            }

            updateTexture();
        }

        bool shouldBeDeleted() const noexcept override {
            return m_healthComponent.isDead() || !m_gameState.inActiveArea(getPosition().x);
        }

        bool isOnPlayerSide() const noexcept {
            return static_cast<bool>(m_flags & Flags::PLAYER_SIDE);
        }

        bool isPassable() const noexcept override {
            return false;
        }

        bool canUsePickups() const noexcept {
            return test(m_flags, Flags::USE_PICKUPS);
        }

        bool hasBomb() const noexcept {
            return m_bombComponent->hasBomb();
        }
    private:
        ShootComponent m_shootComponent;

        std::unique_ptr<ShootControlComponent> m_shootControlComponent;
        std::unique_ptr<MoveComponent> m_moveComponent;
        std::unique_ptr<BombComponent> m_bombComponent;

        std::vector<std::unique_ptr<DeathEffect>> m_deathEffects;

        HealthComponent m_healthComponent;

        Flags m_flags;

        void updateTexture() noexcept {
            Flags flags = m_flags;
            flags |= m_bombComponent->getTextureFlags();

            const auto& texture = m_gameState.getAssets().getAirplaneTexture(flags);
            setTexture(texture);

            auto size = texture.getSize();
            setOrigin(size.x / 2.f, size.y / 2.f);
        }

        void damage() noexcept {
            if (m_healthComponent.damage())
                for (auto& deathEffect : m_deathEffects) 
                    deathEffect->handleDeath();
        }

        void drawAir(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
            if (m_healthComponent.shouldDraw())
                Sprite::drawAir(target, states);
        }

        friend class Builder;
    };
}

#endif