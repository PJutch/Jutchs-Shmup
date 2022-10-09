/* This file is part of Jutchs Shmup.

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
#include "ShootComponent.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;
using sf::Sprite;
using sf::RenderTarget;
using sf::RenderStates;

#include <SFML/System.hpp>
using sf::Event;
using sf::Keyboard;
using sf::Time;
using sf::Mouse;
using sf::seconds;

#include <span>
using std::span;

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::to_string;

#include <memory>
using std::unique_ptr;

Player::Player(const Texture& texture, const Texture& healthTexture, const Texture& bulletTexture, 
               span<Texture> digitTextures,
               vector<unique_ptr<Entity>>& entities,
               float gameHeight, Vector2f screenSize) noexcept : 
        Airplane{{0.f, 0.f}, texture, 
                 unique_ptr<ShootComponent>(new VolleyShootComponent{*this, gameHeight, entities, 
                                            bulletTexture, *this}), *this, gameHeight},
        m_view{{-gameHeight / 2.f, -gameHeight / 2.f, 
                gameHeight * screenSize.x / screenSize.y, gameHeight}}, 
        m_screenSize{screenSize},
        m_healthTexture{healthTexture}, m_health{3}, m_digitTextures{digitTextures}, m_score{0} {
    m_sprite.setRotation(90.f);
}

void Player::handleMouseButtonPressed(sf::Event::MouseButtonEvent event) {
    if (event.button == Mouse::Left) {
        m_shootComponent->tryShoot(true);
    }
}

void Player::update(Time elapsedTime) noexcept {
    if (isDead()) return;

    m_shootComponent->update(elapsedTime);
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) m_shootComponent->tryShoot(true);

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

void Player::drawGui(RenderTarget& target, RenderStates states) const noexcept {
    Sprite healthSprite{m_healthTexture};
    healthSprite.scale(2, 2);
    for (int i = 0; i < m_health; ++ i) {
        healthSprite.setPosition(2 * i * m_healthTexture.getSize().x, 0.01 * m_screenSize.y);
        target.draw(healthSprite, states);
    }

    string score = to_string(m_score);
    for (int i = 0; i < ssize(score); ++ i) {
        Sprite digitSprite{m_digitTextures[score[i] - '0']};
        digitSprite.setPosition(i * m_digitTextures[0].getSize().x, 0);
        target.draw(digitSprite, states);
    }
}
