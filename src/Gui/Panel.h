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

#include "Gui/Element.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>

namespace Gui {
    class Panel : public Element {
    public:
        Panel(sf::FloatRect rect, sf::Color color) noexcept : 
                m_shape{{rect.width, rect.height}} {
            m_shape.setPosition(rect.left, rect.top);
            m_shape.setFillColor(color);
        }

        void addChild(std::unique_ptr<Element>&& element) noexcept {
            m_children.push_back(std::move(element));
        }

        template<std::derived_from<Element> ElementT, typename... Args>
        void emplaceChild(Args&&... args) noexcept {
            m_children.emplace_back(new ElementT{std::forward<Args>(args)...});
        }
    private:
        sf::RectangleShape m_shape;
        std::vector<std::unique_ptr<Element>> m_children;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
            target.draw(m_shape, states);

            states.transform.translate(m_shape.getPosition());
            for (const auto& child : m_children) {
                target.draw(*child, states);
            }
        }
    };
}

#endif