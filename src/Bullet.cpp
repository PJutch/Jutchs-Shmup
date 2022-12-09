/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Bullet.h"  

#include "Airplane/Airplane.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

Bullet::Bullet(bool playerSide, sf::Vector2f position, GameState& gameState) noexcept :
        Sprite{gameState},
        m_playerSide{playerSide}, m_alive{true} {
    auto& texture = gameState.getAssets().getBulletTexture();
    setTexture(texture);

    auto size = texture.getSize();
    setOrigin(size.x / 2.f, size.y / 2.f);

    setPosition(position);
    setRotation(90.f);
}

void Bullet::acceptCollide(Airplane::Airplane& other) noexcept {
    if (other.isOnPlayerSide() != m_playerSide) 
        m_alive = false;
}
