/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef HEALTH_PICKUP_
#define HEALTH_PICKUP_

#include "Pickup.h"
#include "Airplane/Airplane.h"
#include "SoundEffect.h"

class HealthPickup : public Pickup {
public:
    HealthPickup(sf::Vector2f position, GameState& gameState) noexcept;

    void apply(Airplane::Airplane& airplane) noexcept override {
        if (isAlive() && airplane.addHealth(1)) {
            m_gameState.addSound(m_gameState.getAssets()
                                    .getRandomPowerUpSound(m_gameState.getRandomEngine()));
            die();
        }
    };
};

#endif