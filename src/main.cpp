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
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <utility>
using std::swap;

int main(int argc, char** argv) {
    auto videoMode = sf::VideoMode::getDesktopMode();
    sf::Vector2f screenSize(videoMode.width, videoMode.height);

    sf::RenderWindow window{videoMode, "Jutchs Shmup", sf::Style::Fullscreen};
    window.setVerticalSyncEnabled(true);

    GameState gameState{screenSize};

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            gameState.handleEvent(event);
        }

        if (gameState.shouldEnd()) window.close();

        gameState.update();

        window.clear(sf::Color::Red);
        window.draw(gameState);
        window.display();
    }
    return EXIT_SUCCESS;
}
