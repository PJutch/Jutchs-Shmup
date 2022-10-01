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
        Airplane{position, texture, bulletTexture, entities, gameHeight} {
    m_sprite.setRotation(-90.f);
}

void Enemy::update(Time elapsedTime) noexcept {
    m_shootCooldown -= elapsedTime;
    tryShoot(false);

    float movedX = 250.f * elapsedTime.asSeconds();
    m_sprite.move(-movedX, 0.f);
}