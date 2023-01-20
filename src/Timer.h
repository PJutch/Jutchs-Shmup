/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef TIMER_H_
#define TIMER_H_

#include <SFML/System.hpp>

class OnceTimer {
public:
    OnceTimer() noexcept : m_time{sf::Time::Zero} {}

    void start(sf::Time duration) noexcept {
        m_time = duration;
    }

    void extend(sf::Time duration) noexcept {
        m_time += duration;
    }

    void reset() noexcept {
        m_time = sf::Time::Zero;
    }

    void update(sf::Time elapsedTime) noexcept {
        m_time -= elapsedTime;
    }

    bool isFinished() const noexcept {
        return m_time <= sf::Time::Zero;
    }
private:
    sf::Time m_time;
};

#endif