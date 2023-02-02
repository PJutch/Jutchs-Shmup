/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "AnimatedParticle.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <span>
using std::span;

using std::ssize;

AnimatedParticle::AnimatedParticle(span<const sf::Texture> animation, sf::Time delay) noexcept :
        m_animation{animation}, m_delay{delay}, m_timer{std::ssize(m_animation)} {        
    setTexture(m_animation[0]);
    m_timer.wait(m_delay);
}

void AnimatedParticle::update(sf::Time elapsedTime) noexcept {
    m_timer.update(elapsedTime);
    m_timer.forEachStep([this](int){
        return m_delay;
    });

    if (!m_timer.isReachedMaxStep())
        setTexture(m_animation[m_timer.getStep()]);
}
