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

class Airplane;
class Bullet;
class Pickup;
class GameState;

class Entity : public sf::Drawable {
public:
    Entity(GameState& gameState) noexcept;
    virtual ~Entity() = default;

    void handleEvent(sf::Event event) noexcept {}

    virtual void update(sf::Time elapsedTime) noexcept = 0;

    virtual sf::FloatRect getGlobalBounds() const noexcept = 0;

    virtual void startCollide(Entity& other) noexcept = 0;
    virtual void acceptCollide(Airplane& other) noexcept = 0;
    virtual void acceptCollide(Bullet& other) noexcept = 0;
    virtual void acceptCollide(Pickup& other) noexcept = 0;

    virtual bool shouldBeDeleted() const noexcept = 0;
protected:
    GameState& m_gameState;
};

#endif