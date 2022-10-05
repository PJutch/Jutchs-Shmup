/* This file is part of Jutchs Shmup.

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

#include <span>
#include <vector>

class Player : public Airplane {
public:
    Player(const sf::Texture& texture, const sf::Texture& healthTexture, const sf::Texture& bulletTexture, 
           std::span<sf::Texture> digitTextures,
           std::vector<std::unique_ptr<Entity>>& entities, 
           float gameHeight, sf::Vector2f screenSize) noexcept;

    const sf::View& getView() const noexcept {
        return m_view;
    }

    int getScore() const noexcept {
        return m_score;
    }

    void setScore(int score) noexcept {
        m_score = score;
    }

    void handleMouseButtonPressed(sf::Event::MouseButtonEvent event);

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept {
        return false;
    }

    void drawGui(sf::RenderTarget& target, 
        sf::RenderStates states=sf::RenderStates::Default) const noexcept;

    void handleDamaged() noexcept override {
        -- m_health;
    }

    bool addHealth(int health) noexcept override {
        if (isDead() || m_health >= 3) return false;

        m_health = std::min(m_health + health, 3);
        return true;
    }

    bool isDead() const noexcept {
        return m_health <= 0;
    }

    void reset() noexcept {
        m_view.move(-m_sprite.getPosition().x, 0);
        m_sprite.setPosition(0, 0);
        m_health = 3;
        m_score = 0;
    }
private:
    sf::View m_view;
    sf::Vector2f m_screenSize;

    const sf::Texture& m_healthTexture;
    int m_health;

    std::span<sf::Texture> m_digitTextures;
    int m_score;
};

#endif