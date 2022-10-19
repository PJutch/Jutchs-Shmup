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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>

class Player;

class Airplane : public Entity {
public:
    template<std::derived_from<Airplane> T>
    class Builder {
    public:
        template<typename... Args>
        Builder(GameState& gameState, Args&&... args);

        Builder<T>& position(sf::Vector2f position) noexcept {
            m_build->m_sprite.setPosition(position);
            return *this;
        }

        template<std::derived_from<ShootComponent> Component, typename... Args>
        Builder<T>& shootComponent(Args&&... args) noexcept {
            m_build->m_shootComponent.reset(
                new Component{*m_build, m_gameState, std::forward<Args>(args)...});
            return *this;
        }

        template<std::derived_from<ShootControlComponent> Component, typename... Args>
        Builder<T>& shootControlComponent(Args&&... args) noexcept {
            m_build->m_shootControlComponent.reset(
                new Component{std::forward<Args>(args)...});
            return *this;
        }

        template<std::derived_from<MoveComponent> Component, typename... Args>
        Builder<T>& moveComponent(Args&&... args) noexcept {
            m_build->m_moveComponent.reset(
                new Component{*m_build, m_gameState, std::forward<Args>(args)...});
            return *this;
        }

        std::unique_ptr<T> build() noexcept {
            m_build->m_shootControlComponent
                ->registerShootComponent(m_build->m_shootComponent.get());
            return std::move(m_build);
        };
    private:
        std::unique_ptr<T> m_build;
        GameState& m_gameState;
    };

    Airplane(GameState& gameState) noexcept;

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
        if (shouldBeDeleted()) return;
        other.acceptCollide(*this);
    }

    void acceptCollide(Airplane& other) noexcept override {
        if (shouldBeDeleted()) return;
        handleDamaged();
        other.handleDamaged();
    }

    void acceptCollide(Bullet& other) noexcept override {
        if (shouldBeDeleted()) return;
        handleDamaged();
        other.die();
    }

    void acceptCollide(Pickup& other) noexcept override {
        if (shouldBeDeleted()) return;
        other.apply(*this);
    }

    virtual void handleDamaged() noexcept = 0;
    virtual bool addHealth(int health) noexcept = 0; // return true if success

    void update(sf::Time elapsedTime) noexcept override {
        m_shootComponent->update(elapsedTime);
        m_shootControlComponent->update(elapsedTime);
        m_moveComponent->update(elapsedTime);
    }
protected:
    sf::Sprite m_sprite;

    std::unique_ptr<ShootComponent> m_shootComponent;
    std::unique_ptr<ShootControlComponent> m_shootControlComponent;
    std::unique_ptr<MoveComponent> m_moveComponent;

    void setTexture(const sf::Texture& texture) {
        m_sprite.setTexture(texture);
        auto size = texture.getSize();
        m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
};

template<std::derived_from<Airplane> T>
template<typename... Args>
Airplane::Builder<T>::Builder(GameState& gameState, Args&&... args) : 
    m_build{new T{gameState, std::forward<Args>(args)...}}, m_gameState{gameState} {}

#endif