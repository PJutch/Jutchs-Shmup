/* This file is part of JCyberEvolution.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_H_
#define AIRPLANE_H_

#include "Entity.h"
#include "Bullet.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Airplane : public Entity {
public:
    Airplane(sf::Vector2f position, const sf::Texture& texture, const sf::Texture& bulletTexture, 
             std::vector<std::unique_ptr<Entity>>& entities, float gameHeight) noexcept;


    virtual ~Airplane() = default;

    sf::Vector2f getPosition() const noexcept {
        return m_sprite.getPosition();
    }
protected:
    sf::Sprite m_sprite;
    float m_gameHeight;
    sf::Time m_shootCooldown;

    void tryShoot(bool right) noexcept {
        if (m_shootCooldown <= sf::Time::Zero) {
            m_entities.emplace_back(new Bullet{right, m_sprite.getPosition(), m_bulletTexture});
            m_shootCooldown = sf::seconds(0.25f);
        }
    }
private:
    std::vector<std::unique_ptr<Entity>>& m_entities;
    const sf::Texture& m_bulletTexture;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        target.draw(m_sprite, states);
    }
};

#endif