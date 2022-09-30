#include "Player.h"

#include <SFML/Graphics.hpp>
using sf::Texture;

#include <SFML/System.hpp>
using sf::Event;
using sf::Keyboard;
using sf::Time;

Player::Player(const Texture& texture, float gameHeight) : 
        m_sprite{texture}, m_gameHeight{gameHeight}, m_size{texture.getSize()} {
    m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setRotation(90.f);
}

void Player::update(Time elapsedTime) noexcept {
    if (Keyboard::isKeyPressed(Keyboard::W)) {
        m_sprite.move(0, -500.f * elapsedTime.asSeconds());

        if (m_sprite.getGlobalBounds().top < -m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, -m_gameHeight / 2 + m_size.y / 2.f);
    } else if (Keyboard::isKeyPressed(Keyboard::S)) {
        m_sprite.move(0, 500.f * elapsedTime.asSeconds());

        auto globalBounds = m_sprite.getGlobalBounds();
        if (globalBounds.top + globalBounds.width > m_gameHeight / 2)
            m_sprite.setPosition(m_sprite.getPosition().x, m_gameHeight / 2 - m_size.y / 2.f);
    }
}
