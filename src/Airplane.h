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

#include "Entity.h"
#include "Bullet.h"
#include "Pickup.h"
#include "ShootComponent.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>

class Player;

class Airplane : public Entity {
public:
    Airplane(sf::Vector2f position, const sf::Texture& texture, 
             std::unique_ptr<ShootComponent>&& shootComponent,
             Player& player, float gameHeight) noexcept;


    virtual ~Airplane() = default;

    sf::Vector2f getPosition() const noexcept {
        return m_sprite.getPosition();
    }

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
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
protected:
    sf::Sprite m_sprite;
    std::unique_ptr<ShootComponent> m_shootComponent;

    float m_gameHeight;
    Player& m_player;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
};

#endif