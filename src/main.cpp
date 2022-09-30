/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Player.h"

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::RenderWindow;
using sf::Texture;
using sf::View;
using sf::FloatRect;
using sf::Sprite;
using sf::IntRect;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Clock;
using sf::Time;
using sf::seconds;

#include <SFML/Window.hpp>
using sf::Event;
using sf::Keyboard;
using sf::VideoMode;
using sf::Clipboard;
namespace Style = sf::Style;

int main(int argc, char** argv) {
    auto videoMode = VideoMode::getDesktopMode();
    float width = static_cast<float>(videoMode.width);
    float height = static_cast<float>(videoMode.height);

    RenderWindow window{videoMode, "Jutchs Shmup", Style::Fullscreen};
    window.setVerticalSyncEnabled(true);

    Texture playerTexture;
    if (!playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png")) return 1;

    float gameHeight = 512;
    View gameView{{-gameHeight / 2, -gameHeight / 2, gameHeight * width / height, gameHeight}};

    Player player{playerTexture, gameHeight};

    Clock clock;
    while (window.isOpen()) {
        Time elapsedTime = clock.restart();

        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed: window.close(); break;
                case Event::KeyPressed:
                    switch (event.key.code) {
                        case Keyboard::Escape: window.close(); break;
                    }
            }
        }

        player.update(elapsedTime);

        window.clear(Color::Green);
        window.setView(gameView);

        window.draw(player);

        window.display();
    }
    return EXIT_SUCCESS;
}
