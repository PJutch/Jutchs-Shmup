/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Player.h"
#include "Bullet.h"
#include "Airplane.h"
#include "Enemy.h"
#include "Entity.h"
#include "GameState.h"

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::RenderWindow;
using sf::Texture;
using sf::View;
using sf::FloatRect;
using sf::Sprite;
using sf::IntRect;
using sf::Vector2f;
using sf::Vector2u;

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

#include <format>
using std::format;

#include <vector>
using std::vector;

#include <array>
using std::array;

using std::ssize;

#include <random>
using std::random_device;
using std::mt19937_64;
using std::uniform_real_distribution;

#include <memory>
using std::unique_ptr;

#include <utility>
using std::swap;

int main(int argc, char** argv) {
    auto videoMode = VideoMode::getDesktopMode();
    Vector2f screenSize(videoMode.width, videoMode.height);

    RenderWindow window{videoMode, "Jutchs Shmup", Style::Fullscreen};
    window.setVerticalSyncEnabled(true);

    GameState gameState{screenSize};

    float spawnX = gameState.getGameHeight() * 4;

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
                    break;
            }

            gameState.handleEvent(event);
        }

        gameState.update(elapsedTime);

        window.clear(Color::Green);
        window.draw(gameState);
        window.display();
    }
    return EXIT_SUCCESS;
}
