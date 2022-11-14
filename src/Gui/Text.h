/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_TEXT_H_
#define GUI_TEXT_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <string>

namespace Gui {
    class Text : public Element {
    public:
        Text(const std::string& string, const sf::Font& font, 
                    int characterSize, sf::Color color) noexcept : 
                m_string{string} {
            m_text.setFillColor(color);
            m_text.setFont(font);
            m_text.setCharacterSize(characterSize);
            m_text.setString(sf::String::fromUtf8(m_string.begin(), m_string.end()));
        }
        
        sf::Vector2f getSize() const noexcept {
            auto localBounds = m_text.getLocalBounds();
            return {localBounds.width, localBounds.height};
        }

        void setPosition(sf::Vector2f position) noexcept {
            m_text.setPosition(position);
        }

        void setOrigin(sf::Vector2f origin) noexcept {
            m_text.setOrigin(origin);
        }
    private:
        sf::Text m_text;
        const std::string& m_string;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
            target.draw(m_text, states);
        }
    };
}

#endif