/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_MOVE_COMPONENT_H_
#define AIRPLANE_MOVE_COMPONENT_H_

#include <SFML/System.hpp>

namespace Airplane {
    class MoveComponent {
    public:
        MoveComponent() noexcept : m_speed{0.f, 0.f} {}
        virtual ~MoveComponent() = default;

        virtual void update(sf::Time elapsedTime) noexcept = 0;

        virtual sf::Vector2f getMinSpeed() const noexcept = 0;
    protected:
        sf::Vector2f m_speed;

        sf::Vector2f getMoved(sf::Time elapsedTime) const noexcept {
            return elapsedTime.asSeconds() * m_speed;
        }
    private:
        friend class Builder;
    };
}

#endif