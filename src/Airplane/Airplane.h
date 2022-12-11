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

#include "GameState.h"
#include "Sprite.h"
#include "Bullet.h"
#include "Bomb.h"
#include "Airplane/ShootComponent.h"
#include "Airplane/ShootControlComponent.h"
#include "Airplane/MoveComponent.h"
#include "Airplane/DeathEffect.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>
#include <algorithm>

namespace Airplane {
    class Airplane : public Sprite<Airplane> {
    public:
        friend class Builder;

        void handleEvent(sf::Event event) noexcept override {
            m_shootControlComponent->handleEvent(event);

            if (test(m_flags, Flags::HAS_BOMBS) 
             && event.type == sf::Event::MouseButtonPressed 
             && event.mouseButton.button == sf::Mouse::Right) {
                m_gameState.getEntities().addEntity(new Bomb{getPosition(), m_gameState});
            }
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

        // return true if success
        bool addHealth(int health) noexcept { 
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

        sf::Vector2f getMinSpeed() const noexcept {
            return m_moveComponent->getMinSpeed();
        }

        const ShootComponent& getShootComponent() const noexcept {
            return *m_shootComponent;
        }

        ShootComponent& getShootComponent() noexcept {
            return *m_shootComponent;
        }

        void update(sf::Time elapsedTime) noexcept override {
            if (isDead() || shouldBeDeleted()) return;

            m_damageCooldown -= elapsedTime;
            
            m_shootComponent->update(elapsedTime);
            m_shootControlComponent->update(elapsedTime);
            m_moveComponent->update(elapsedTime);

            if (m_shootControlComponent->shouldShoot()) {
                m_shootComponent->tryShoot();
            }
        }

        bool isActive() const noexcept override {
            return !isDead() && !shouldBeDeleted();
        }

        bool shouldBeDeleted() const noexcept override {
            return test(m_flags, Flags::DELETABLE)
                && (m_health <= 0 || !m_gameState.inActiveArea(getPosition().x));
        }

        bool reset() noexcept override {
            if (test(m_flags, Flags::DELETABLE)) return true;

            m_health = m_maxHealth;
            setPosition(0.f, 0.f);
            return false;
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
            return static_cast<bool>(m_flags & Flags::USE_PICKUPS);
        }
    private:
        std::unique_ptr<ShootComponent> m_shootComponent;
        std::unique_ptr<ShootControlComponent> m_shootControlComponent;
        std::unique_ptr<MoveComponent> m_moveComponent;

        std::vector<std::unique_ptr<DeathEffect>> m_deathEffects;

        int m_health;
        int m_maxHealth;
        sf::Time m_damageCooldown;

        Flags m_flags;

        Airplane(GameState& gameState) noexcept : 
            Sprite{gameState},
            m_health{0}, m_maxHealth{0}, m_damageCooldown{sf::seconds(0.f)} {}
    };
}

#endif