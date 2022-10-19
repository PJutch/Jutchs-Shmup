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
    Player(GameState& GameState) noexcept;

    const sf::View& getView() const noexcept {
        return m_view;
    }

    int getScore() const noexcept {
        return m_score;
    }

    void setScore(int score) noexcept {
        m_score = score;
    }

    void addScore(int score) noexcept {
        m_score += score;
    }

    void handleMouseButtonPressed(sf::Event::MouseButtonEvent event);

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
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

    void setPosition(sf::Vector2f position) noexcept override {
        m_view.move(position.x - m_sprite.getPosition().x, 0.f);
        Airplane::setPosition(position);
    }

    void move(sf::Vector2f offset) noexcept override {
        Airplane::move(offset);
        m_view.move(offset.x, 0.f);
    }

    void reset() noexcept {
        m_view.move(-m_sprite.getPosition().x, 0);
        m_sprite.setPosition(0, 0);
        m_health = 3;
        m_score = 0;
    }
private:
    sf::View m_view;
    int m_health;
    int m_score;
};

#endif