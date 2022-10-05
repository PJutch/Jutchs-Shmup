#include "Pickup.h"

Pickup::Pickup(sf::Vector2f position, const sf::Texture& texture) noexcept : 
        m_sprite{texture}, m_alive{true} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
}
