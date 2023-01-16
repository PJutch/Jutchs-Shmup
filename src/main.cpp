/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "GameState.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <stdexcept>

#include <utility>
using std::swap;

int main(int argc, char** argv) {
    try {
        auto videoMode = sf::VideoMode::getDesktopMode();
        sf::Vector2f screenSize(videoMode.width, videoMode.height);

        sf::RenderWindow window{videoMode, "Jutchs Shmup", sf::Style::Fullscreen};
        window.setVerticalSyncEnabled(true);

        sf::Image icon;
        if (!icon.loadFromFile("resources/textures/icon.png")) {
            throw TextureLoadError{"Can't load window icon"};
        }

        auto [x, y] = icon.getSize();
        window.setIcon(x, y, icon.getPixelsPtr());

        GameState gameState{screenSize};
        window.clear(sf::Color::Black);
        gameState.drawLoadingScreen(window);
        window.display();
        gameState.init();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                gameState.handleEvent(event);
            }

            if (gameState.shouldEnd()) window.close();

            gameState.update();

            window.clear(sf::Color::Black);
            window.draw(gameState);
            window.display();
        }
    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        throw;
    }

    return EXIT_SUCCESS;
}
