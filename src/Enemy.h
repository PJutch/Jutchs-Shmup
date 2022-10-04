/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ENEMY_H_
#define ENEMY_H_

#include "Airplane.h"
#include "Entity.h"
#include "Player.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>

class Enemy : public Airplane {
public:
    Enemy(sf::Vector2f position, Player& player, 
          const sf::Texture& texture, const sf::Texture& bulletTexture, 
          std::vector<std::unique_ptr<Entity>>& entities,
          float gameHeight) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive;
    }

    void handleDamaged() noexcept override {
        m_alive = false;
        m_player.setScore(m_player.getScore() + 10);
    }
private:
    bool m_alive;
    Player& m_player;
};

#endif