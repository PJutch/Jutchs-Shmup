/* This file is part of JCyberEvolution.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Player.h"

#include "Bullet.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Event;
using sf::Keyboard;
using sf::Time;
using sf::Mouse;
using sf::seconds;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

Player::Player(const Texture& texture, const Texture& bulletTexture, 
               vector<unique_ptr<Entity>>& entities,
               float gameHeight, Vector2f screenSize) noexcept : 
        m_sprite{texture}, m_gameHeight{gameHeight}, 
        m_view{{-gameHeight / 2.f, -gameHeight / 2.f, 
                gameHeight * screenSize.x / screenSize.y, gameHeight}}, 
        m_shootCooldown{Time::Zero}, m_entities{entities}, m_bulletTexture{bulletTexture} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setRotation(90.f);
}

void Player::handleMouseButtonPressed(sf::Event::MouseButtonEvent event) {
    if (event.button == Mouse::Left && m_shootCooldown <= Time::Zero) {
        m_entities.emplace_back(new Bullet{true, m_sprite.getPosition(), m_bulletTexture});
        m_shootCooldown = seconds(0.25f);
    }
}

void Player::update(Time elapsedTime) noexcept {
    m_shootCooldown -= elapsedTime;

    if (Keyboard::isKeyPressed(Keyboard::W)) {
        m_sprite.move(0, -250.f * elapsedTime.asSeconds());

        auto globalBounds = m_sprite.getGlobalBounds();
        if (globalBounds.top < -m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, 
                                 -m_gameHeight / 2 + globalBounds.height / 2.f);
    } else if (Keyboard::isKeyPressed(Keyboard::S)) {
        m_sprite.move(0, 250.f * elapsedTime.asSeconds());

        auto globalBounds = m_sprite.getGlobalBounds();
        if (globalBounds.top + globalBounds.width > m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, 
                                 m_gameHeight / 2 - globalBounds.height / 2.f);
    }

    float movedX = 250.f * elapsedTime.asSeconds();
    if (Keyboard::isKeyPressed(Keyboard::D)) movedX *= 2;

    m_sprite.move(movedX, 0.f);
    m_view.move(movedX, 0);
}
