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
    class Airplane : public CollidableBase<Airplane, Sprite> {
    public:
        // use Airplane::Builder instead
        Airplane(GameState& gameState) noexcept : 
            Base{gameState},  m_health{0}, m_maxHealth{0}, m_damageCooldown{sf::seconds(0.f)} {}

        friend class Builder;

        void handleEvent(sf::Event event) noexcept override {
            m_shootControlComponent->handleEvent(event);
            m_bombComponent->handleEvent(event);
        }

        void acceptCollide(Airplane& other) noexcept override {
            handleDamaged();
        }

        void acceptCollide(Bullet& other) noexcept override {
            if (other.isOnPlayerSide() == isOnPlayerSide()) return;
            handleDamaged();
        }

        void handleDamaged() noexcept {
            if (m_damageCooldown <= sf::Time::Zero) {
                m_damageCooldown = sf::seconds(0.1f);
                if (-- m_health <= 0) 
                    for (auto& deathEffect : m_deathEffects) 
                        deathEffect->handleDeath();
            }
        }

        // return true on success
        bool tryAddHealth(int health) noexcept { 
            if (m_health >= m_maxHealth) return false;
            m_health = std::min(m_health + health, m_maxHealth);
            return true;
        }

        void setHealth(int health) noexcept {
            m_health = health;
        }

        int getHealth() const noexcept {
            return m_health;
        }

        bool tryAddBomb() noexcept {
            return m_bombComponent->tryAddBomb();
        }

        sf::Vector2f getMinSpeed() const noexcept {
            return m_moveComponent->getMinSpeed();
        }

        sf::FloatRect getShootGlobalAffectedArea() const noexcept {
            return m_shootComponent->getGlobalAffectedArea();
        }

        void update(sf::Time elapsedTime) noexcept override {
            m_damageCooldown -= elapsedTime;
            
            m_shootComponent->update(elapsedTime);
            m_moveComponent->update(elapsedTime);
            m_bombComponent->update(elapsedTime);

            if (m_shootControlComponent->shouldShoot()) {
                m_shootComponent->tryShoot();
            }

            updateTexture();
        }

        bool isActive() const noexcept override {
            return !shouldBeDeleted();
        }

        bool shouldBeDeleted() const noexcept override {
            return m_health <= 0 || !m_gameState.inActiveArea(getPosition().x);
        }

        bool isDead() const noexcept {
            return m_health <= 0;
        }

        bool isOnPlayerSide() const noexcept {
            return static_cast<bool>(m_flags & Flags::PLAYER_SIDE);
        }

        bool isPassable() const noexcept override {
            return !isActive();
        }

        bool canUsePickups() const noexcept {
            return test(m_flags, Flags::USE_PICKUPS);
        }

        bool hasBomb() const noexcept {
            return m_bombComponent->hasBomb();
        }
    private:
        std::unique_ptr<ShootComponent> m_shootComponent;
        std::unique_ptr<ShootControlComponent> m_shootControlComponent;
        std::unique_ptr<MoveComponent> m_moveComponent;
        std::unique_ptr<BombComponent> m_bombComponent;

        std::vector<std::unique_ptr<DeathEffect>> m_deathEffects;

        int m_health;
        int m_maxHealth;
        sf::Time m_damageCooldown;

        Flags m_flags;

        void updateTexture() noexcept {
            Flags flags = m_flags;
            flags |= m_bombComponent->getTextureFlags();

            const auto& texture = m_gameState.getAssets().getAirplaneTexture(flags);
            setTexture(texture);

            auto size = texture.getSize();
            setOrigin(size.x / 2.f, size.y / 2.f);
        }
    };
}

#endif