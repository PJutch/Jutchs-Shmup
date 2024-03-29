/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#include "Timer.h"

#include "GameState.h"

PassedTimer::PassedTimer(GameState& gameState) noexcept : 
    m_gameState{gameState}, m_start{gameState.getCurrentTime()} {}
    
sf::Time PassedTimer::getPassedTime() const noexcept {
    return m_gameState.getCurrentTime() - m_start;
}
