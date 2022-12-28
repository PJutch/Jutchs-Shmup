/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ANIMATED_PARTICLE_H_
#define ANIMATED_PARTICLE_H_

#include "Sprite.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <span>

class AnimatedParticle : public CollidableBase<AnimatedParticle, Sprite> {
public:
    AnimatedParticle(GameState& gameState, sf::Vector2f position, std::span<const sf::Texture> animation, 
                     sf::Time delay) noexcept;

    void update(sf::Time elapsedTime) noexcept override;
    
    bool isPassable() const noexcept override {
        return true;
    }

    bool shouldBeDeleted() const noexcept override {
        return m_currentTexture >= std::ssize(m_animation);
    }
private:
    std::span<const sf::Texture> m_animation;
    sf::Time m_delay;

    int m_currentTexture;
    sf::Time m_untilNext;
};

#endif