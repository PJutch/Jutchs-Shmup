/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef BOMB_H_
#define BOMB_H_

#include "GameState.h"
#include "Sprite.h"
#include "AnimatedParticle.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Bomb : public Sprite<Bomb> {
public:
    Bomb(bool playerSide, sf::Vector2f position, GameState& gameState);

    void update(sf::Time elapsedTime) override;

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(getPosition().x));
    }

    bool isPassable() const noexcept override {
        return true;
    }
private:
    sf::Time m_launched;
    bool m_alive;
};

#endif