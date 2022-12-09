/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "Entity.h"
#include "GameState.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// CRTP
template <typename Child>
class Sprite : public EntityBase<Child> {
public:
    Sprite(GameState& gameState) noexcept : m_gameState{gameState} {}

    sf::FloatRect getGlobalBounds() const noexcept override {
        return m_sprite.getGlobalBounds();
    }

    sf::Vector2f getPosition() const noexcept {
        return m_sprite.getPosition();
    }

    void setPosition(sf::Vector2f position) noexcept {
        m_sprite.setPosition(position);
    }

    void setPosition(float x, float y) noexcept {
        setPosition({x, y});
    }

    void move(sf::Vector2f offset) noexcept {
        m_sprite.move(offset);
    }
    
    void move(float x, float y) noexcept {
        move({x, y});
    }
protected:
    GameState& m_gameState;

    void setTexture(const sf::Texture& texture) noexcept {
        m_sprite.setTexture(texture);
    }

    void setOrigin(sf::Vector2f origin) noexcept {
        m_sprite.setOrigin(origin);
    }

    void setOrigin(float x, float y) noexcept {
        setOrigin({x, y});
    }

    void setRotation(float rotation) noexcept {
        m_sprite.setRotation(rotation);
    }
private:
    sf::Sprite m_sprite;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
};

#endif