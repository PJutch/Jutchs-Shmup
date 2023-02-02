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

class Turret : public CollidableBase<Turret> {
public:
    Turret(GameState& gameState, sf::Vector2f position) noexcept : m_gameState{gameState} {
        const sf::Texture& baseTexture = m_gameState.getAssets().getTurretBaseTexture();
        m_base.setTexture(baseTexture);
        auto [baseSizeX, baseSizeY] = baseTexture.getSize();
        m_base.setOrigin(baseSizeX / 2.f, baseSizeY / 2.f);

        const sf::Texture& turretTexture = m_gameState.getAssets().getTurretTexture();
        m_turret.setTexture(turretTexture);
        auto [sizeX, sizeY] = turretTexture.getSize();
        m_turret.setOrigin(sizeX / 2.f, sizeY / 2.f);

        m_base.setPosition(position);
        m_turret.setPosition(position);
    }

    void update(sf::Time) noexcept override {}

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_base.getGlobalBounds();
    }

    bool isPassable() const noexcept override {
        return true;
    }

    bool shouldBeDeleted() const noexcept override {
        return !m_gameState.inActiveArea(m_base.getPosition().x);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_base, states);
        target.draw(m_turret, states);
    }
private:
    sf::Sprite m_base;
    sf::Sprite m_turret;

    GameState& m_gameState;
};

#endif