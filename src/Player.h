/* This file is part of JCyberEvolution.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "Airplane.h"
#include "Entity.h"

#include <SFML/Graphics.hpp>

#include <vector>

class Player : public Airplane {
public:
    Player(const sf::Texture& texture, const sf::Texture& bulletTexture, 
           std::vector<std::unique_ptr<Entity>>& entities, 
           float gameHeight, sf::Vector2f screenSize) noexcept;

    const sf::View& getView() const noexcept {
        return m_view;
    }

    void handleMouseButtonPressed(sf::Event::MouseButtonEvent event);

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept {
        return false;
    }
private:
    sf::View m_view;
};

#endif