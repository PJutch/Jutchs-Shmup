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

#include "Airplane.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Time;

Bullet::Bullet(Airplane* owner, bool playerSide, Vector2f position, GameState& gameState) noexcept :
        Entity{gameState}, m_sprite{gameState.getAssets().getBulletTexture()}, 
        m_playerSide{playerSide}, m_alive{true}, m_owner{owner} {
    auto size = gameState.getAssets().getBulletTexture().getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
    m_sprite.setRotation(90.f);
}

void Bullet::update(Time elapsedTime) noexcept {
    if (!m_alive) return;

    m_sprite.move((m_playerSide ? 1 : -1) * 750.f * elapsedTime.asSeconds(), 0);
}

void Bullet::acceptCollide(Airplane& other) noexcept {
    if (!m_alive) return;

    if (&other != m_owner) {
        die();
        other.handleDamaged();
    }
}
