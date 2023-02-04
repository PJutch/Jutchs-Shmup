/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#include "TurretBullet.h"

#include "Airplane/Airplane.h"

#include "geometry.h"

TurretBullet::TurretBullet(GameState& gameState, 
    sf::Vector2f position, sf::Vector2f direction) noexcept :
        Sprite{gameState}, m_speed{direction * 750.f}, 
        m_liveTimer{gameState}, m_alive{true} {
    const sf::Texture& texture = gameState.getAssets().getBulletTexture();
    setTexture(texture);

    sf::Vector2u size = texture.getSize();
    setOrigin(size.x / 2.f, size.y / 2.f);

    setPosition(position);

    setRotation(-to_deegrees(std::atan2(direction.x, direction.y)) - 180.f);
}

bool TurretBullet::shouldBeDeleted() const noexcept {
    auto [x, y] = getPosition();
    float gameHeight = m_gameState.getGameHeight();
    return !(m_alive 
          && (-gameHeight / 2.f <= y && y <= gameHeight / 2.f) 
          && m_gameState.inActiveArea(x) 
          && m_liveTimer.getPassedTime() <= sf::seconds(1.f));
}

void TurretBullet::update(sf::Time elapsedTime) noexcept {
    move(m_speed * elapsedTime.asSeconds());
    
    if (isAtMaxHeight())
        setScale(1.f);
    else {
        float t = m_liveTimer.getPassedTime().asSeconds();
        float height = VERTICAL_SPEED * t - GRAVITY * t * t / 2;
        setScale(1.f / (2.f - height));
    }
}

void TurretBullet::acceptCollide(Airplane::Airplane& other) noexcept {
    if (isAtMaxHeight() && other.isOnPlayerSide())
        m_alive = false;
}
