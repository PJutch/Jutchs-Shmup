/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GUI_MANAGER_
#define GUI_MANAGER_

#include "Panel.h"
#include "Text.h"

#include <SFML/Graphics.hpp>

class GameState;

namespace Gui {
    class Manager : public sf::Drawable {
    public:
        Manager(GameState& gameState);

        void initGui();

        bool isMenuOpen() const {
            return m_menuOpen;
        }

        void handleEvent(const sf::Event& event);

        void update(sf::Time elapsedTime);

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void drawLoadingScreen(sf::RenderTarget& target, sf::RenderStates states) const;

        void reset() noexcept {
            m_loadingDots = 0;
            m_loadingDotsChangeDelay = LOADING_DOTS_CHANGE_DELAY;
        }
    private:
        GameState& m_gameState;
        bool m_menuOpen;
        Gui::Panel m_menu;

        Gui::Text m_loadingText;
        int m_loadingDots;
        sf::Time m_loadingDotsChangeDelay;

        Gui::Text m_bestScoreText;

        const static inline sf::Time LOADING_DOTS_CHANGE_DELAY = sf::seconds(0.1f);

        // return element size
        // element's origin at its top center point
        sf::Vector2f addMenuText(sf::Vector2f position);
        sf::Vector2f addVolumeSlider(sf::Vector2f position);
        sf::Vector2f addLanguageCombo(sf::Vector2f position);

        // return element size
        // element's origin at its bottom center point
        sf::Vector2f addMenuButtons(sf::Vector2f position);

        // return element size
        // element's origin at its center point
        sf::Vector2f createLoadingText(sf::Vector2f position);

        // return element size
        // element's origin at its top center point
        sf::Vector2f createBestScoreText(sf::Vector2f position);

        void setBestScoreText(const std::string& text);
        
        // return element size
        // element's origin at its top left corner
        sf::Vector2f drawHealth(sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states) const;
    };
}

#endif