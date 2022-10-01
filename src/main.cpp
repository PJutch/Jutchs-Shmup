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
#include "Enemy.h"
#include "Entity.h"

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

#include <vector>
using std::vector;

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

    Texture playerTexture;
    if (!playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0000.png")) return 1;

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png")) return 1;

    Texture enemyTexture;
    if (!enemyTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png")) return 1;

    mt19937_64 randomEngine(random_device{}());

    float gameHeight = 512;

    vector<unique_ptr<Entity>> entities;

    auto* player = new Player{playerTexture, bulletTexture, entities, gameHeight, screenSize};
    entities.emplace_back(player);

    float spawnX = gameHeight * 4;

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

        for (int i = 0; i < entities.size(); ++ i) 
            entities[i]->update(elapsedTime);
        
        for (auto& entity : entities) {
            if (entity->shouldBeDeleted()) {
                swap(entity, entities.back());
                entities.pop_back();
            }
        }

        while (player->getPosition().x + 4 * gameHeight > spawnX) {
            Vector2u enemySize = enemyTexture.getSize();
            for (float y = (enemySize.y - gameHeight) / 2; 
                    y < (gameHeight- enemySize.y) / 2; y += enemySize.y) {
                if (uniform_real_distribution(0.0, 1.0)(randomEngine) < 0.01) {
                    entities.emplace_back(
                        new Enemy{{spawnX, y}, enemyTexture, bulletTexture, entities, gameHeight});
                }
            }
            spawnX += enemySize.x;
        }

        window.clear(Color::Green);
        window.setView(player->getView());

        for (auto& entity : entities) window.draw(*entity);

        window.display();
    }
    return EXIT_SUCCESS;
}
