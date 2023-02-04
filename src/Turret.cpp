/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#include "Turret.h"

#include "TurretBullet.h"

#include "geometry.h"

#include <cmath>

Turret::Turret(GameState& gameState, sf::Vector2f position) noexcept : 
        m_alive{true}, m_gameState{gameState} {
    const sf::Texture& baseTexture = m_gameState.getAssets().getTurretBaseTexture();
    m_base.setTexture(baseTexture);
    sf::Vector2u baseSize = baseTexture.getSize();
    m_base.setOrigin(baseSize.x / 2.f, baseSize.y / 2.f);

    const sf::Texture& turretTexture = m_gameState.getAssets().getTurretTexture();
    m_turret.setTexture(turretTexture);
    sf::Vector2u size = turretTexture.getSize();
    m_turret.setOrigin(size.x / 2.f, size.y / 2.f);

    m_base.setPosition(position);
    m_turret.setPosition(position);
}

void Turret::update(sf::Time elapsedTime) noexcept {
    auto& entities = m_gameState.getEntities();

    sf::Vector2f playerPosition = entities.getPlayerPosition();
    sf::Vector2f playerDirection = normalize(playerPosition - m_turret.getPosition());

    m_turret.setRotation(-to_deegrees(std::atan2(playerDirection.x, playerDirection.y)));

    m_shootCooldown.update(elapsedTime);
    if (m_shootCooldown.isFinished()) {
        entities.addEntity<TurretBullet>(m_turret.getPosition(), playerDirection);
        m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
        m_shootCooldown.start(sf::seconds(1.0f));
    }
}

void Turret::handleBombExplosion(sf::Vector2f position, float radius) {
    float baseRadius = m_gameState.getAssets().getTurretBaseTexture().getSize().x;
    float distance = length(position - m_base.getPosition());
    
    if (distance <= radius + baseRadius) {
        m_alive = false;
        m_gameState.getScoreManager().addScore(100);
    }
}
