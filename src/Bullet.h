#ifndef BULLET_H_
#define BULLET_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Bullet : public sf::Drawable {
public:
    Bullet(sf::Vector2f position, const sf::Texture& texture) noexcept;

    void update(sf::Time elapsedTime) noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
private:
    sf::Sprite m_sprite;
    bool moveRight;
};

#endif