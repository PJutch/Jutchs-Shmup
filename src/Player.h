#ifndef PLAYER_H_
#define PLAYER_H_

#include "Ship.h"
#include "Bullet.h"

#include <SFML/Graphics.hpp>

#include <vector>

class Player : public sf::Drawable, public Ship {
public:
    Player(std::vector<Bullet>& bullets, 
           const sf::Texture& texture, const sf::Texture& bulletTexture, 
           float gameHeight, sf::Vector2f screenSize) noexcept;

    const sf::View& getView() const noexcept {
        return m_view;
    }

    void handleMouseButtonPressed(sf::Event::MouseButtonEvent event);

    void update(sf::Time elapsedTime) noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
private:
    sf::Sprite m_sprite;
    float m_gameHeight;

    sf::View m_view;

    sf::Time m_shootCooldown;
    std::vector<Bullet>& m_bullets;
    const sf::Texture& m_bulletTexture;
};

#endif