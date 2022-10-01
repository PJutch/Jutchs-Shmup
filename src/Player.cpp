#include "Player.h"

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

Player::Player(vector<Bullet>& bullets, 
               const Texture& texture, const Texture& bulletTexture, 
               float gameHeight, Vector2f screenSize) noexcept : 
        m_sprite{texture}, m_gameHeight{gameHeight}, 
        m_view{{-gameHeight / 2.f, -gameHeight / 2.f, 
                gameHeight * screenSize.x / screenSize.y, gameHeight}}, 
        m_shootCooldown{Time::Zero}, m_bullets{bullets}, m_bulletTexture{bulletTexture} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setRotation(90.f);
}

void Player::handleMouseButtonPressed(sf::Event::MouseButtonEvent event) {
    if (event.button == Mouse::Left && m_shootCooldown <= Time::Zero) {
        m_bullets.emplace_back(m_sprite.getPosition(), m_bulletTexture);
        m_shootCooldown = seconds(0.25f);
    }
}

void Player::update(Time elapsedTime) noexcept {
    m_shootCooldown -= elapsedTime;

    if (Keyboard::isKeyPressed(Keyboard::W)) {
        m_sprite.move(0, -500.f * elapsedTime.asSeconds());

        auto globalBounds = m_sprite.getGlobalBounds();
        if (globalBounds.top < -m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, 
                                 -m_gameHeight / 2 + globalBounds.height / 2.f);
    } else if (Keyboard::isKeyPressed(Keyboard::S)) {
        m_sprite.move(0, 500.f * elapsedTime.asSeconds());

        auto globalBounds = m_sprite.getGlobalBounds();
        if (globalBounds.top + globalBounds.width > m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, 
                                 m_gameHeight / 2 - globalBounds.height / 2.f);
    }

    float movedX = 500.f * elapsedTime.asSeconds();
    if (Keyboard::isKeyPressed(Keyboard::D)) movedX *= 2;

    m_sprite.move(movedX, 0.f);
    m_view.move(movedX, 0);
}
