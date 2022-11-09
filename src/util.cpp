/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "util.h"

sf::Event moveEvent(sf::Event event, sf::Vector2f offset) noexcept {
    switch (event.type) {
    case sf::Event::MouseWheelScrolled:
        event.mouseWheelScroll.x += offset.x;
        event.mouseWheelScroll.y += offset.y;
        break;
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
        event.mouseButton.x += offset.x;
        event.mouseButton.y += offset.y;
        break;
    case sf::Event::MouseMoved:
        event.mouseMove.x += offset.x;
        event.mouseMove.y += offset.y;
        break;
    case sf::Event::TouchBegan:
    case sf::Event::TouchMoved:
    case sf::Event::TouchEnded:
        event.touch.x += offset.x;
        event.touch.y += offset.y;
        break;
    }

    return event;
}