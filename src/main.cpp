/* This file is part of JCyberEvolution.

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

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

int main(int argc, char** argv) {
    auto videoMode = VideoMode::getDesktopMode();
    Vector2f screenSize(videoMode.width, videoMode.height);

    RenderWindow window{videoMode, "Jutchs Shmup", Style::Fullscreen};
    window.setVerticalSyncEnabled(true);

    Texture playerTexture;
    if (!playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png")) return 1;

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png")) return 1;

    float gameHeight = 512;

    vector<Bullet> bullets;
    vector<unique_ptr<Airplane>> airplanes;

    Player* player = new Player{bullets, playerTexture, bulletTexture, gameHeight, screenSize};
    airplanes.emplace_back(player);

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
                case Event::MouseButtonPressed:
                    player->handleMouseButtonPressed(event.mouseButton);
                    break;
            }
        }

        player->update(elapsedTime);
        for (auto& bullet : bullets) bullet.update(elapsedTime);

        window.clear(Color::Green);
        window.setView(player->getView());

        window.draw(*player);
        for (auto& bullet : bullets) window.draw(bullet);

        window.display();
    }
    return EXIT_SUCCESS;
}
