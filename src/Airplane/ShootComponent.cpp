/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootComponent.h"

#include "Airplane.h"

#include "../SoundEffect.h"
#include "../Bullet.h"

#include "../algorithm.h"
#include "../geometry.h"

#include <algorithm>
#include <ranges>
#include <cmath>

namespace Airplane {
    void ShootComponent::setPattern(Pattern pattern) {
        m_pattern = pattern;
        m_currentElement = std::ssize(pattern);

        if (m_pattern.empty()) {
            m_localAffectedArea = {0.f, 0.f, 0.f, 0.f};
            return;
        }

        sf::Vector2f bulletSize = Bullet::getSize(m_gameState);

        auto offsets = pattern | std::views::transform(&PatternElement::offset);
        float minX = min_value(offsets | std::views::transform(&sf::Vector2f::x));
        float minY = min_value(offsets | std::views::transform(&sf::Vector2f::y));
        float maxY = max_value(offsets | std::views::transform(&sf::Vector2f::y));

        m_localAffectedArea = {minX - bulletSize.x / 2.f, minY - bulletSize.x / 2.f, 
                               INFINITY, maxY - minY + bulletSize.x};
    }

    void ShootComponent::update(sf::Time elapsedTime) {
        m_shootCooldown -= elapsedTime;
        
        bool shot = false;
        while (m_shootCooldown <= sf::Time::Zero 
            && m_currentElement < std::ssize(m_pattern)) {
            spawnBullet(m_pattern[m_currentElement].offset);

            m_shootCooldown += m_pattern[m_currentElement].delay;
            ++ m_currentElement;

            shot = true;
        }

        if (shot) shotSound();
    }

    void ShootComponent::spawnBullet(sf::Vector2f offset) const {
        m_gameState.getEntities().addEntity<Bullet>(m_owner.isOnPlayerSide(), 
                                                    m_owner.getPosition() + offset);
    }

    void ShootComponent::shotSound() const {
        m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
    }

    sf::FloatRect ShootComponent::getGlobalAffectedArea() const noexcept {
        auto [top, left, width, height] = m_localAffectedArea;
        auto [x, y] = m_owner.getPosition();

        if (m_owner.isOnPlayerSide())
            return {left + x, top + y,  width,  height};
        else
            return {left - x, top - y, -width, -height};
    }
}
