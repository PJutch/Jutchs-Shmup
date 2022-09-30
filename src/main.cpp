/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include <SFML/Graphics.hpp>
using sf::Color;
using sf::RenderWindow;
using sf::Texture;
using sf::View;
using sf::FloatRect;

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

    Clock clock;
    Time timeBeforeUpdate = Time::Zero;
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
    
        window.clear(Color::White);
        window.display();
    }
    return EXIT_SUCCESS;
}
