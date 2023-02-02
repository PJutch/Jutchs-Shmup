/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef TURRET_H_
#define TURRET_H_

#include "Sprite.h"
#include "Entity.h"

#include <SFML/System.hpp>

class Turret : public Sprite, public CollidableBase<Turret> {
public:
    Turret(GameState& gameState, sf::Vector2f position) noexcept : Sprite{gameState} {
        const sf::Texture& texture = m_gameState.getAssets().getTurretTexture();
        setTexture(texture);

        auto [sizeX, sizeY] = texture.getSize();
        setOrigin(sizeX / 2.f, sizeY / 2.f);

        setPosition(position);
    }

    void update(sf::Time) noexcept override {}

    virtual bool isPassable() const noexcept {
        return true;
    }

    virtual bool shouldBeDeleted() const noexcept {
        return !m_gameState.inActiveArea(getPosition().x);
    }
};

#endif