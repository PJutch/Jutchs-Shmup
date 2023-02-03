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

#include "Entity.h"
#include "GameState.h"

#include "Timer.h"
#include "geometry.h"

#include <SFML/System.hpp>

#include <cmath>

class Turret : public CollidableBase<Turret> {
public:
    Turret(GameState& gameState, sf::Vector2f position) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_base.getGlobalBounds();
    }

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(m_base.getPosition().x));
    }

    void drawLand(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_base, states);
        target.draw(m_turret, states);
    }

    void handleBombExplosion(sf::Vector2f position, float radius);
private:
    sf::Sprite m_base;
    sf::Sprite m_turret;

    bool m_alive;

    OnceTimer m_shootCooldown;

    GameState& m_gameState;
};

#endif