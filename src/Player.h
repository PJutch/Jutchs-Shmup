/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "Airplane.h"
#include "Entity.h"

#include <SFML/Graphics.hpp>

#include <span>
#include <vector>

class Player : public Airplane {
public:
    Player(GameState& GameState) noexcept;

    bool shouldBeDeleted() const noexcept override {
        return false;
    }

    void handleKilled() noexcept override {}
};

#endif