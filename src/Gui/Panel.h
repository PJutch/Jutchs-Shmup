/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_PANEL_H_
#define GUI_PANEL_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Gui {
    class Panel : public sf::Drawable {
    public:
        Panel(sf::FloatRect area) : m_shape{{area.width, area.height}} {
            m_shape.setPosition(area.left, area.top);
            m_shape.setFillColor({0, 0, 0, 128});
        }
    private:
        sf::RectangleShape m_shape;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
            target.draw(m_shape, states);
        }
    };
}

#endif