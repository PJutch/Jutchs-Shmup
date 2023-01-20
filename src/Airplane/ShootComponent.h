/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_SHOOT_COMPONENT_H_
#define AIRPLANE_SHOOT_COMPONENT_H_

#include "../Timer.h"

#include "../declarations.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <span>

namespace Airplane {
    class ShootComponent {
    public:
        struct PatternElement {
            sf::Vector2f offset;
            sf::Time delay;
        };
        using Pattern = std::span<PatternElement>;

        ShootComponent(Airplane& owner, GameState& gameState) noexcept :
            m_owner{owner}, m_gameState{gameState} {}

        void update(sf::Time elapsedTime);

        void trySetShouldShoot() noexcept {
            m_shootTimer.tryRestart();
        }

        // width  can be infinite
        // height can be infinite and/or negative
        sf::FloatRect getGlobalAffectedArea() const noexcept;
    private:
        StepTimer m_shootTimer;

        Pattern m_pattern;

        // in local coordinate space
        // respect rotation (+x is move direction)
        // width can be infinite
        sf::FloatRect m_localAffectedArea;

        Airplane& m_owner;
        GameState& m_gameState;

        void spawnBullet(sf::Vector2f offset) const;
        void shotSound() const;

        void setPattern(Pattern pattern);

        friend class Builder;
    };
}

#endif