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

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;
}

class Bullet;
class Pickup;
class AnimatedParticle;

class GameState;

class Entity : public sf::Drawable {
public:
    Entity(GameState& gameState) noexcept : m_gameState{gameState} {}
    virtual ~Entity() = default;

    void handleEvent(sf::Event event) noexcept {}

    virtual void update(sf::Time elapsedTime) noexcept = 0;

    virtual sf::FloatRect getGlobalBounds() const noexcept = 0;

    virtual void startCollide(Entity& other) noexcept = 0;   

    virtual void acceptCollide(Airplane::Airplane& other) noexcept {}
    virtual void acceptCollide(Bullet& other) noexcept {}
    virtual void acceptCollide(Pickup& other) noexcept {}
    virtual void acceptCollide(AnimatedParticle& other) noexcept {}

    virtual bool isPassable() const noexcept = 0;

    virtual bool shouldBeDeleted() const noexcept = 0;

    // return true if should be deleted after reset
    virtual bool reset() noexcept { 
        return true;
    }
protected:
    GameState& m_gameState;
};

#endif