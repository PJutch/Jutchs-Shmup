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

#include <memory>
using std::unique_ptr;

Enemy::Enemy(Vector2f position,
             const Texture& texture, const Texture& bulletTexture, 
             vector<unique_ptr<Entity>>& entities,
             float gameHeight) noexcept : 
        m_sprite{texture}, m_alive{true}, m_entities{entities},
        m_bulletTexture{bulletTexture}, m_shootCooldown{Time::Zero}, m_gameHeight{gameHeight} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
    m_sprite.setRotation(-90.f);
}

void Enemy::update(Time elapsedTime) noexcept {
    m_shootCooldown -= elapsedTime;
    if (m_shootCooldown <= Time::Zero) {
        m_entities.emplace_back(new Bullet{false, m_sprite.getPosition(), m_bulletTexture});
        m_shootCooldown = seconds(0.25f);
    }

    float movedX = 250.f * elapsedTime.asSeconds();
    m_sprite.move(-movedX, 0.f);
}