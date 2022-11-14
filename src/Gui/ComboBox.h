/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_COMBO_BOX_H_
#define GUI_COMBO_BOX_H_

#include "Gui/Element.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <vector>
#include <memory>
#include <functional>

namespace Gui {
    class ComboBox : public Element {
    public:
        ComboBox(std::function<int ()> getCurrent, std::function<void (int)> setCurrent, 
                 sf::Color fillColor, sf::Color outlineColor, float outlineThickness) noexcept;

        sf::Vector2f getSize() const noexcept {
            auto localBounds = m_shape.getLocalBounds();
            return {localBounds.width, localBounds.height};
        }

        void setRect(sf::FloatRect rect) noexcept {
            setSize({rect.width, rect.height});

            auto origin = m_shape.getOrigin();
            setPosition({rect.left + origin.x, rect.top + origin.y});
        }

        void setSize(sf::Vector2f size) noexcept {
            m_shape.setSize(size);
            updateSize();
        }

        void setPosition(sf::Vector2f position) noexcept {
            m_shape.setPosition(position);
        }

        void setOrigin(sf::Vector2f origin) noexcept {
            m_shape.setOrigin(origin);
            m_listShape.setOrigin(origin);
        }

        void addChild(std::unique_ptr<Element>&& element) noexcept {
            m_children.push_back(std::move(element));
            updateSize();
        }

        template<std::derived_from<Element> ElementT, typename... Args>
        void emplaceChild(Args&&... args) noexcept {
            m_children.emplace_back(new ElementT{std::forward<Args>(args)...});
            updateSize();
        }

        void handleEvent(const sf::Event& event) 
            noexcept(noexcept(m_setCurrent(std::declval<int>()))) override;
    private:
        sf::RectangleShape m_shape;
        sf::RectangleShape m_listShape;

        std::vector<std::unique_ptr<Element>> m_children;

        std::function<int ()> m_getCurrent;
        std::function<void (int)> m_setCurrent;

        bool m_active;

        // unsafe! requires std::ssize(children) > m_getCurrent()
        void draw(sf::RenderTarget& target, sf::RenderStates states) 
            const noexcept(noexcept(m_getCurrent)) override;

        void drawList(sf::RenderTarget& target, sf::RenderStates states) const noexcept;

        void updateText() noexcept override;

        void updateSize() noexcept {
            auto size = m_shape.getSize();
            m_listShape.setSize({size.x, std::ssize(m_children) * size.y});
        }
    };
}

#endif