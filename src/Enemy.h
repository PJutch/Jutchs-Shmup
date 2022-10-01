#ifndef ENEMY_H_
#define ENEMY_H_

#include "Airplane.h"
#include "Bullet.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>

class Enemy : public Airplane {
public:
    Enemy(sf::Vector2f position, std::vector<Bullet>& bullets, 
          const sf::Texture& texture, const sf::Texture& bulletTexture, 
          float gameHeight) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
private:
    sf::Sprite m_sprite;
    bool m_alive;

    std::vector<Bullet>& m_bullets;
    const sf::Texture& m_bulletTexture;
    sf::Time m_shootCooldown;

    float m_gameHeight;
};

#endif