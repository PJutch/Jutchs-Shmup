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
        Panel(sf::Color color) noexcept;

        sf::Vector2f getSize() const noexcept {
            auto localBounds = m_shape.getLocalBounds();
            return {localBounds.width, localBounds.height};
        }

        void setRect(sf::FloatRect rect) noexcept {
            m_shape.setSize({rect.width, rect.height});

            auto origin = m_shape.getOrigin();
            m_shape.setPosition(rect.left + origin.x, rect.top + origin.y);
        }

        void setSize(sf::Vector2f size) noexcept {
            m_shape.setSize(size);
        }

        void setPosition(sf::Vector2f position) noexcept {
            m_shape.setPosition(position);
        }

        void setOrigin(sf::Vector2f origin) noexcept {
            m_shape.setOrigin(origin);
        }

        void addChild(std::unique_ptr<Element>&& element) noexcept {
            m_children.push_back(std::move(element));
        }

        template<std::derived_from<Element> ElementT, typename... Args>
        void emplaceChild(Args&&... args) noexcept {
            m_children.emplace_back(new ElementT{std::forward<Args>(args)...});
        }

        void handleEvent(const sf::Event& event) noexcept override;
    private:
        sf::RectangleShape m_shape;
        std::vector<std::unique_ptr<Element>> m_children;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept;
    };
}

#endif