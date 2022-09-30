#ifndef PLAYER_H_
#define PLAYER_H_

#include <SFML/Graphics.hpp>

class Player : public sf::Drawable {
public:
    Player(const sf::Texture& texture, float gameHeight);

    void update(sf::Time elapsedTime) noexcept;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
        target.draw(m_sprite, states);
    }
private:
    sf::Sprite m_sprite;
    float m_gameHeight;
    sf::Vector2f m_size;
};

#endif