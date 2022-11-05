/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Panel.h"

namespace Gui {
    Panel::Panel(sf::Color color) noexcept {
        m_shape.setFillColor(color);
    }

    void Panel::handleEvent(const sf::Event& event) noexcept {
        sf::Event newEvent = event;

        auto position = m_shape.getPosition();
        switch (newEvent.type) {
        case sf::Event::MouseWheelScrolled:
            newEvent.mouseWheelScroll.x -= position.x;
            newEvent.mouseWheelScroll.y -= position.y;
            break;
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
            newEvent.mouseButton.x -= position.x;
            newEvent.mouseButton.y -= position.y;
            break;
        case sf::Event::MouseMoved:
            newEvent.mouseMove.x -= position.x;
            newEvent.mouseMove.y -= position.y;
            break;
        case sf::Event::TouchBegan:
        case sf::Event::TouchMoved:
        case sf::Event::TouchEnded:
            newEvent.touch.x -= position.x;
            newEvent.touch.y -= position.y;
            break;
        }

        for (auto& child : m_children) {
            child->handleEvent(newEvent);
        }
    }

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
        target.draw(m_shape, states);

        states.transform.translate(m_shape.getPosition());
        for (const auto& child : m_children) {
            target.draw(*child, states);
        }
    }
}
