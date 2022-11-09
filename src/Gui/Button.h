/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_BUTTON_H_
#define GUI_BUTTON_H_

#include "Element.h"
#include "util.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <functional>

namespace Gui {
    class Button : public Element {
    public:
        Button(const std::function<void ()>& action, 
                sf::Color fillColor, sf::Color outlineColor, 
                float outlineThickness, float activeOutlineThickness) noexcept;

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

        void setChild(std::unique_ptr<Element>&& element) noexcept {
            m_child = std::move(element);
        }

        template<std::derived_from<Element> ElementT, typename... Args>
        void emplaceChild(Args&&... args) noexcept {
            m_child.reset(new ElementT{std::forward<Args>(args)...});
        }

        void handleEvent(const sf::Event& event) noexcept override;
    private:
        std::function<void ()> m_action;

        sf::RectangleShape m_shape;
        std::unique_ptr<Element> m_child;

        float m_outlineThickness;
        float m_activeOutlineThickness;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
            target.draw(m_shape, states);

            if (m_child) {
                states.transform.translate(m_shape.getPosition());
                target.draw(*m_child, states);
            }
        }
    };
}

#endif