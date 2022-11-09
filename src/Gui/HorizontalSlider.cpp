/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Gui/HorizontalSlider.h"

namespace Gui {
    HorizontalSlider::HorizontalSlider(sf::Color runnerColor, sf::Color lineColor) noexcept :
            m_active{false} {
        m_runner.setFillColor(runnerColor);
        m_line.setFillColor(lineColor);
    }

    void HorizontalSlider::handleEvent(const sf::Event& event) noexcept {
        switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
                if (m_runner.getGlobalBounds().contains(
                        event.mouseButton.x, event.mouseButton.y)) {
                    m_active = true;
                } else if (m_line.getGlobalBounds().contains(
                        event.mouseButton.x, event.mouseButton.y)) {
                    m_active = true;

                    float left = m_line.getGlobalBounds().left;
                    m_runner.setPosition(event.mouseButton.x - left, 0.f);
                }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
                m_active = false;
        case sf::Event::MouseMoved:
            if (m_active) {
                float x = event.mouseMove.x;
                x -= m_line.getGlobalBounds().left;
                if (x < 0.f) 
                    x = 0.f;
                if (x > m_line.getGlobalBounds().width) 
                    x = m_line.getGlobalBounds().width;

                m_runner.setPosition(x, 0.f);
            }
            break;
        }
    }
}