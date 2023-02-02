/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Bomb.h"

Bomb::Bomb(GameState& gameState, bool playerSide, sf::Vector2f position) :
        Sprite{gameState}, m_launched{gameState.getCurrentTime()}, m_alive{true} {
    auto& texture = gameState.getAssets().getBombTexture();
    setTexture(texture);

    auto size = texture.getSize();
    setOrigin(size.x / 2.f, size.y / 2.f);

    setPosition(position);
    setRotation(playerSide ? 90.f : -90.f);
}

void Bomb::update(sf::Time elapsedTime) {
    float t = (m_gameState.getCurrentTime() - m_launched).asSeconds();

    float y = 10.f * t * t;
    if (y > 1.f) {
        m_alive = false;

        auto& entities = m_gameState.getEntities();

        const float radius = 24.f;

        m_gameState.getLand().handleBombExplosion(getPosition());
        entities.handleBombExplosion(getPosition(), radius);

        auto particle = entities.createEntity<AnimatedParticleLand>(
            static_cast<std::span<const sf::Texture>>(m_gameState.getAssets().getExplosionAnimation()), 
            sf::seconds(0.1f));
        particle->setPosition(getPosition());
        particle->setScale(radius / m_gameState.getAssets().getExplosionAnimation()[0].getSize().x);
        entities.addEntity(std::move(particle));

        m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomExplosionSound());
    }
    setScale(1.f / (1.f + y));
}
