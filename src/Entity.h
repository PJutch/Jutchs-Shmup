/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */


#ifndef ENTITY_H_
#define ENTITY_H_

#include "declarations.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Entity : public sf::Drawable {
public:
    virtual ~Entity() = default;

    virtual void handleEvent(sf::Event event) {}

    virtual void update(sf::Time elapsedTime) = 0;

    virtual sf::FloatRect getGlobalBounds() const noexcept = 0;

    virtual bool isActive() const noexcept {
        return !shouldBeDeleted();
    }

    virtual void startCollide(Entity& other) = 0;   
    virtual void acceptCollide(Airplane::Airplane& other) {}
    virtual void acceptCollide(Bullet& other) {}
    virtual void acceptCollide(Bomb& other) {}
    virtual void acceptCollide(Pickup& other) {}
    virtual void acceptCollide(AnimatedParticle& other) {}

    // for AI only
    virtual bool isPassable() const noexcept = 0;

    virtual bool shouldBeDeleted() const noexcept = 0;
};

// CRTP
template <typename Child>
class EntityBase : public Entity {
public:
    void startCollide(Entity& other) noexcept override final {
        other.acceptCollide(static_cast<Child&>(*this));
    }
};

#endif