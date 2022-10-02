/* This file is part of JCyberEvolution.

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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Bullet : public Entity {
public:
    Bullet(Airplane* owner, bool moveRight, sf::Vector2f position, const sf::Texture& texture) noexcept;

    void update(sf::Time elapsedTime) noexcept;

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
    }

    void startCollide(Entity& other) noexcept override {
        other.acceptCollide(*this);
    }

    void acceptCollide(Airplane& other) noexcept override;

    virtual void acceptCollide(Bullet& other) noexcept {}

    bool shouldBeDeleted() const noexcept override {
        return !m_alive;
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