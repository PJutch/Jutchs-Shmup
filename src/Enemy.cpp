#include "Enemy.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;

#include <vector>
using std::vector;

Enemy::Enemy(Vector2f position, vector<Bullet>& bullets, 
             const Texture& texture, const Texture& bulletTexture, 
             float gameHeight) noexcept : 
        m_sprite{texture}, m_alive{true}, m_bullets{bullets},
        m_bulletTexture{bulletTexture}, m_shootCooldown{Time::Zero}, m_gameHeight{gameHeight} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
    m_sprite.setRotation(-90.f);
}

void Enemy::update(Time elapsedTime) noexcept {
    m_shootCooldown -= elapsedTime;
    if (m_shootCooldown <= Time::Zero) {
        m_bullets.emplace_back(false, m_sprite.getPosition(), m_bulletTexture);
        m_shootCooldown = seconds(0.25f);
    }

    float movedX = 500.f * elapsedTime.asSeconds();
    m_sprite.move(-movedX, 0.f);
}