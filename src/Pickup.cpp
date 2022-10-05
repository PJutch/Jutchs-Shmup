#include "Pickup.h"

#include "Player.h"

Pickup::Pickup(sf::Vector2f position, const sf::Texture& texture, 
               const Player& player, float gameHeight) noexcept : 
        m_sprite{texture}, m_alive{true}, m_player{player}, m_gameHeight{gameHeight} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
}

bool Pickup::shouldBeDeleted() const noexcept {
    return !m_alive || m_sprite.getPosition().x + 2 * m_gameHeight < m_player.getPosition().x
                    || m_sprite.getPosition().x - 5 * m_gameHeight > m_player.getPosition().x;
}
