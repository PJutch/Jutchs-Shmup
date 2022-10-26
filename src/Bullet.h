/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef BULLET_H_
#define BULLET_H_

#include "Entity.h"
#include "GameState.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Player;

class Bullet : public Entity {
public:
    Bullet(Airplane* owner, bool moveRight, sf::Vector2f position, GameState& gameState) noexcept;
    
    static sf::Vector2f getSize(GameState& gameState) noexcept {
        auto size = gameState.getBulletTexture().getSize();
        return sf::Vector2f(size.x, size.y);
    }

    void update(sf::Time elapsedTime) noexcept;

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
    }

    void startCollide(Entity& other) noexcept override {
        other.acceptCollide(*this);
    }

    void acceptCollide(Airplane& other) noexcept override;

    void acceptCollide(Bullet&) noexcept override {}
    void acceptCollide(Pickup&) noexcept override {}

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(m_sprite.getPosition().x));
    }

    bool isPassable() const noexcept override {
        return true;
    }

    void die() noexcept {
        m_alive = false;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }

    sf::Sprite m_sprite;

    bool m_moveRight;
    bool m_alive;
    
    Airplane* m_owner;
};

#endif