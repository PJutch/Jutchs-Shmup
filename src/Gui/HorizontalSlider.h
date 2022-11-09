/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_HORIZONTAL_SLIDER_H_
#define GUI_HORIZONTAL_SLIDER_H_

#include "Gui/Element.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <functional>

namespace Gui {
    class HorizontalSlider : public Element {
    public:
        HorizontalSlider(std::function<float ()> getValue, std::function<void (float)> setValue, 
                         sf::Color runnerColor, sf::Color lineColor) noexcept;

        sf::Vector2f getSize() const noexcept {
            return m_line.getSize();
        }

        sf::Vector2f getRunnerSize() const noexcept {
            return m_runner.getSize();
        }

        void setRect(sf::FloatRect rect) noexcept {
            m_line.setSize({rect.width, rect.height});

            auto origin = m_line.getOrigin();
            m_line.setPosition(rect.left + origin.x, rect.top + origin.y);
        }

        void setSize(sf::Vector2f size) noexcept {
            m_line.setSize(size);
        }

        void setPosition(sf::Vector2f position) noexcept {
            m_line.setPosition(position);
        }

        void setOrigin(sf::Vector2f origin) noexcept {
            m_line.setOrigin(origin);
        }

        void setRunnerSize(sf::Vector2f size) noexcept {
            m_runner.setSize(size);
        }

        void setRunnerOrigin(sf::Vector2f origin) noexcept {
            m_runner.setOrigin(origin);
        }

        void handleEvent(const sf::Event& event) 
            noexcept(noexcept(m_setValue(std::declval<float>()))) override;
    private:
        sf::RectangleShape m_runner;
        sf::RectangleShape m_line;

        bool m_active;

        std::function<float ()> m_getValue;
        std::function<void (float)> m_setValue;

        void draw(sf::RenderTarget& target, sf::RenderStates states) 
                const noexcept(noexcept(m_getValue())) override {
            target.draw(m_line, states);

            states.transform.translate(m_line.getPosition() - m_line.getOrigin());
            states.transform.translate(m_getValue() * m_line.getSize().x, 0.f);
            target.draw(m_runner, states);
        }
    };
}

#endif