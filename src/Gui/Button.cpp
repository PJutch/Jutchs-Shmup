/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Gui/Button.h"

namespace Gui {
    Button::Button(const std::function<void ()>& action, 
            sf::Color fillColor, sf::Color outlineColor, 
            float outlineThickness, float activeOutlineThickness) noexcept :
                m_action{action}, m_outlineThickness{outlineThickness}, 
                m_activeOutlineThickness{activeOutlineThickness} {
        m_shape.setFillColor(fillColor);
        m_shape.setOutlineColor(outlineColor);
        m_shape.setOutlineThickness(m_outlineThickness);
    }

    bool Button::handleEvent(const sf::Event& event) noexcept(noexcept(m_action())) {
        if (m_child && m_child->handleEvent(moveEvent(event, -m_shape.getPosition())))
            return true;

        switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left
             && m_shape.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                m_action();
                return true;
            }
        case sf::Event::MouseMoved:
            if (m_shape.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                m_shape.setOutlineThickness(m_activeOutlineThickness);
            } else {
                m_shape.setOutlineThickness(m_outlineThickness);
            }
            break;
        }

        return false;
    }
}
