#include "Bullet.h"   

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Time;

Bullet::Bullet(Vector2f position, const Texture& texture) noexcept : m_sprite{texture} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
    m_sprite.setRotation(90.f);
}

void Bullet::update(Time elapsedTime) noexcept {
    m_sprite.move(1500.f * elapsedTime.asSeconds(), 0);
}
