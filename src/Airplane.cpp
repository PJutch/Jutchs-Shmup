#include "Airplane.h"

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Event;
using sf::Time;
using sf::seconds;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

Airplane::Airplane(Vector2f position, const Texture& texture, const Texture& bulletTexture, 
                   vector<unique_ptr<Entity>>& entities, float gameHeight) noexcept : 
        m_sprite{texture}, m_gameHeight{gameHeight}, 
        m_shootCooldown{Time::Zero}, m_entities{entities}, m_bulletTexture{bulletTexture} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
}
