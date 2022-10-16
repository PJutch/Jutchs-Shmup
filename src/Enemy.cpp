/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Enemy.h"

#include "Bullet.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;

#include <vector>
using std::vector;

#include <random>
using std::mt19937_64;

#include <memory>
using std::unique_ptr;

Enemy::Enemy(Vector2f position, GameState& gameState) noexcept : 
        Airplane{position, gameState.getEnemyTexture(), gameState}, m_alive{true} {
    m_sprite.setRotation(-90.f);
}

void Enemy::update(Time elapsedTime) noexcept {
    if (!m_alive) return;
    
    m_shootComponent->update(elapsedTime);
    m_shootComponent->tryShoot(false);

    float movedX = 250.f * elapsedTime.asSeconds();
    m_sprite.move(-movedX, 0.f);
}
