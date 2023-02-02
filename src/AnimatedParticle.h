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
#include "Timer.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <span>

class AnimatedParticle : public CollidableBase<AnimatedParticle> {
public:
    void update(sf::Time elapsedTime) noexcept override;

    sf::FloatRect getGlobalBounds() const noexcept {
        return m_sprite.getGlobalBounds();
    }
    
    bool isPassable() const noexcept override {
        return true;
    }

    bool shouldBeDeleted() const noexcept override {
        return m_timer.isReachedMaxStep();
    }

    void setPosition(sf::Vector2f position) noexcept {
        m_sprite.setPosition(position);
    }

    void setPosition(float x, float y) noexcept {
        setPosition({x, y});
    }

    void setScale(sf::Vector2f factors) noexcept {
        m_sprite.setScale(factors);
    }

    void setScale(float xFactor, float yFactor) noexcept {
        setScale({xFactor, yFactor});
    }

    void setScale(float scale) noexcept {
        setScale(scale, scale);
    }
protected:
    AnimatedParticle(std::span<const sf::Texture> animation, sf::Time delay) noexcept;

    void draw(sf::RenderTarget& target, 
              sf::RenderStates states = sf::RenderStates::Default) const noexcept {
        target.draw(m_sprite, states);
    }
private:
    sf::Sprite m_sprite;

    std::span<const sf::Texture> m_animation;
    sf::Time m_delay;

    StepTimer m_timer;

    void setTexture(const sf::Texture& texture) noexcept {
        m_sprite.setTexture(texture);

        auto size = texture.getSize();
        m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    }
};

class AnimatedParticleAir : public AnimatedParticle {
public:
    AnimatedParticleAir(std::span<const sf::Texture> animation, sf::Time delay) noexcept :
        AnimatedParticle(animation, delay) {}

    void drawAir(sf::RenderTarget& target, 
              sf::RenderStates states = sf::RenderStates::Default) const noexcept override {
        draw(target, states);
    }
};

class AnimatedParticleLand : public AnimatedParticle {
public:
    AnimatedParticleLand(std::span<const sf::Texture> animation, sf::Time delay) noexcept :
        AnimatedParticle(animation, delay) {}

    void drawLand(sf::RenderTarget& target, 
              sf::RenderStates states = sf::RenderStates::Default) const noexcept override {
        draw(target, states);
    }
};

static_assert(sizeof(AnimatedParticleLand{std::declval<std::span<const sf::Texture>>(), 
                                          std::declval<sf::Time>()}) != 0);

#endif