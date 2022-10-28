/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_H_
#define AIRPLANE_H_

#include "GameState.h"
#include "Entity.h"
#include "Bullet.h"
#include "Pickup.h"
#include "ShootComponent.h"
#include "ShootControlComponent.h"
#include "MoveComponent.h"
#include "DeathEffect.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>
#include <algorithm>

class Airplane : public Entity {
public:
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

        Builder& texture(const sf::Texture& texture) noexcept {
            m_build->m_sprite.setTexture(texture);

            auto size = texture.getSize();
            m_build->m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);

            return *this;
        }

        Builder& speed(sf::Vector2f speed) noexcept {
            m_speed = speed;
            return *this;
        }

        Builder& playerSide(bool playerSide) noexcept {
            m_build->m_playerSide = playerSide;
            m_build->m_sprite.setRotation(playerSide ? 90.f : -90.f);
            return *this;
        }

        template<std::derived_from<ShootComponent> Component, typename... Args>
        Builder& shootComponent(Args&&... args) noexcept {
            m_build->m_shootComponent.reset(
                new Component{*m_build, m_gameState, std::forward<Args>(args)...});
            return *this;
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        std::unique_ptr<ShootControlComponent> createShootControlComponent(Args&&... args) noexcept {
            return std::make_unique<Component>(*m_build, m_gameState, std::forward<Args>(args)...);
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        Builder& shootControlComponent(Args&&... args) noexcept {
            m_build->m_shootControlComponent
                = createShootControlComponent<Component>(std::forward<Args>(args)...);
            return *this;
        }

        template<std::derived_from<MoveComponent> Component, typename... Args>
        Builder& moveComponent(Args&&... args) noexcept {
            m_build->m_moveComponent.reset(
                new Component{*m_build, m_gameState, std::forward<Args>(args)...});
            return *this;
        }

        template<std::derived_from<DeathEffect> Component, typename... Args>
        Builder& addDeathEffect(Args&&... args) noexcept {
            m_build->m_deathEffects.emplace_back(
                new Component{*m_build, m_gameState, std::forward<Args>(args)...});
            return *this;
        }

        std::unique_ptr<Airplane> build() noexcept {
            m_build->m_moveComponent->setSpeed(m_speed);
            return std::move(m_build);
        };
    private:
        std::unique_ptr<Airplane> m_build;
        sf::Vector2f m_speed;
        bool m_shootRight;

        GameState& m_gameState;
    };

    Airplane(GameState& gameState) noexcept;

    void handleEvent(sf::Event event) noexcept {
        m_shootControlComponent->handleEvent(event);
    }

    sf::Vector2f getPosition() const noexcept {
        return m_sprite.getPosition();
    }

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
    }

    virtual void setPosition(sf::Vector2f position) noexcept {
        m_sprite.setPosition(position);
    }

    virtual void move(sf::Vector2f offset) noexcept {
        m_sprite.move(offset);
    }

    void startCollide(Entity& other) noexcept override {
        if (isDead() || shouldBeDeleted()) return;
        other.acceptCollide(*this);
    }

    void acceptCollide(Airplane& other) noexcept override {
        if (isDead() || shouldBeDeleted()) return;
        handleDamaged();
        other.handleDamaged();
    }

    void acceptCollide(Bullet& other) noexcept override {
        if (other.isOnPlayerSide() == m_playerSide || isDead() || shouldBeDeleted()) return;
        handleDamaged();
        other.die();
    }

    void acceptCollide(Pickup& other) noexcept override {
        if (isDead() || shouldBeDeleted()) return;
        other.apply(*this);
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

    bool shouldBeDeleted() const noexcept override {
        return m_deletable 
            && (m_health <= 0 || !m_gameState.inActiveArea(m_sprite.getPosition().x));
    }

    bool isDead() const noexcept {
        return m_health <= 0;
    }

    bool isOnPlayerSide() const noexcept {
        return m_playerSide;
    }

    bool isPassable() const noexcept override {
        return false;
    }
protected:
    sf::Sprite m_sprite;

    std::unique_ptr<ShootComponent> m_shootComponent;
    std::unique_ptr<ShootControlComponent> m_shootControlComponent;
    std::unique_ptr<MoveComponent> m_moveComponent;

    std::vector<std::unique_ptr<DeathEffect>> m_deathEffects;

    int m_health;
    int m_maxHealth;
    sf::Time m_damageCooldown;

    bool m_playerSide;

    bool m_deletable;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        if (m_health > 0) target.draw(m_sprite, states);
    }
};

template<typename... Args>
Airplane::Builder::Builder(GameState& gameState, Args&&... args) : 
    m_build{new Airplane{gameState, std::forward<Args>(args)...}}, m_gameState{gameState} {}

#endif