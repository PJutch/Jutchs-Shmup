/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

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

Airplane::Airplane(Vector2f position, const Texture& texture, GameState& gameState) noexcept : 
        Entity{gameState}, m_sprite{texture}, m_shootComponent{nullptr}, m_shootControlComponent{nullptr} {
    auto size = texture.getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setPosition(position);
}
