/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef PICKUP_H_
#define PICKUP_H_

#include "Entity.h"
#include "Bullet.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Airplane;
class Player;

class Pickup : public Entity {
public:
    Pickup(sf::Vector2f position, const sf::Texture& texture, 
           const Player& player, float gameHeight) noexcept;

    virtual ~Pickup() = default;

    sf::Vector2f getPosition() const noexcept {
        return m_sprite.getPosition();
    }

    void update(sf::Time) noexcept override {}

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
    }

    void startCollide(Entity& other) noexcept override {
        other.acceptCollide(*this);
    }

    void acceptCollide(Airplane& other) noexcept override {
        apply(other);
    }

    void acceptCollide(Bullet&) noexcept override {}
    void acceptCollide(Pickup&) noexcept override {}

    virtual void apply(Airplane& airplane) noexcept = 0;

    bool shouldBeDeleted() const noexcept override;
protected:
    void die() noexcept {
        m_alive = false;
    }

    bool isAlive() const noexcept {
        return m_alive;
    }
private:
    sf::Sprite m_sprite;

    bool m_alive;

    const Player& m_player;
    float m_gameHeight;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
};

#endif