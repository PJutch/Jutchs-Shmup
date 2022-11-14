/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ComboBox.h"

#include "util.h"

namespace Gui {
    ComboBox::ComboBox(std::function<int ()> getCurrent, std::function<void (int)> setCurrent, 
                       sf::Color fillColor, sf::Color outlineColor, float outlineThickness) noexcept :
            m_getCurrent{getCurrent}, m_setCurrent{setCurrent}, m_active{false} {
        m_shape.setFillColor(fillColor);
        m_shape.setOutlineColor(outlineColor);
        m_shape.setOutlineThickness(outlineThickness);

        m_listShape.setFillColor(fillColor);
        m_listShape.setOutlineColor(outlineColor);
        m_listShape.setOutlineThickness(outlineThickness);
    }

    bool ComboBox::handleEvent(const sf::Event& event) 
            noexcept(noexcept(m_setCurrent(std::declval<int>()))) {
        sf::Event newEvent = moveEvent(event, -m_shape.getPosition());
        if ((m_children[m_getCurrent()])->handleEvent(newEvent)) return true;
        if (m_active) {
            newEvent = moveEvent(event, {0.f, m_shape.getSize().y});
            for (auto& child : m_children) {
                newEvent = moveEvent(event, {0.f, m_shape.getSize().y});
                if (child->handleEvent(newEvent)) return true;
            }
        }

        switch (event.type) {
        case sf::Event::MouseButtonPressed: {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (m_active) {
                mousePos -= m_shape.getPosition();
                mousePos.y -= m_shape.getSize().y;
                if (m_listShape.getGlobalBounds().contains(mousePos)) {
                    m_setCurrent((mousePos.y + m_listShape.getOrigin().y) / m_shape.getSize().y);
                    return true;
                }
                m_active = false;
            } else {
                if (m_shape.getGlobalBounds().contains(mousePos)) m_active = true;
            }
            break;
        }
        }

        return false;
    }

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) 
            const noexcept(noexcept(m_getCurrent)) {
        target.draw(m_shape, states);

        states.transform.translate(m_shape.getPosition());
        if (m_active) drawList(target, states);
        target.draw(*(m_children[m_getCurrent()]), states);
    }

    void ComboBox::drawList(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
        states.transform.translate(0.f, m_shape.getSize().y);
        target.draw(m_listShape, states);

        for (const auto& child : m_children) {
            target.draw(*child, states);
            states.transform.translate(0.f, m_shape.getSize().y);
        }
    }
}
