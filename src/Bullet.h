/* This file is part of Jutchs Shmup.

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

#include "GameState.h"
#include "Sprite.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;
}

class Bullet : public Sprite<Bullet> {
public:
    Bullet(GameState& gameState, bool playerSide, sf::Vector2f position);
    
    static sf::Vector2f getSize(GameState& gameState) noexcept {
        auto size = gameState.getAssets().getBulletTexture().getSize();
        return sf::Vector2f(size.x, size.y);
    }

    static sf::Vector2f getSpeed() noexcept {
        return {750.f, 0.f};
    }

    void update(sf::Time elapsedTime) noexcept override {
        move((m_playerSide ? 1 : -1) * 750.f * elapsedTime.asSeconds(), 0);
    }

    void acceptCollide(Airplane::Airplane& other) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(getPosition().x));
    }

    bool isPassable() const noexcept override {
        return true;
    }

    bool isOnPlayerSide() const noexcept {
        return m_playerSide;
    }
private:
    bool m_playerSide;
    bool m_alive;
};

#endif