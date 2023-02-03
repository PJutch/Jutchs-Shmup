/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef PICKUP_H_
#define PICKUP_H_

#include "Entity.h"
#include "Airplane/Airplane.h"
#include "GameState.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Pickup : public Sprite, public CollidableBase<Pickup> {
public:
    Pickup(GameState& gameState, sf::Vector2f position, const sf::Texture& texture) noexcept;

    virtual ~Pickup() = default;

    void update(sf::Time) noexcept override {}

    void acceptCollide(Airplane::Airplane& other) noexcept override {
        if (other.canUsePickups()) apply(other);
    }

    virtual void apply(Airplane::Airplane& airplane) noexcept = 0;

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(getPosition().x));
    }
protected:
    void die() noexcept {
        m_alive = false;
    }
private:
    bool m_alive;
};

class HealthPickup : public Pickup {
public:
    HealthPickup(GameState& gameState, sf::Vector2f position) noexcept: 
        Pickup{gameState, position, gameState.getAssets().getHealthPickupTexture()} {}

    void apply(Airplane::Airplane& airplane) noexcept override {
        if (airplane.tryHeal()) {
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomPowerUpSound());
            die();
        }
    };
};

class BombPickup : public Pickup {
public:
    BombPickup(GameState& gameState, sf::Vector2f position) noexcept: 
        Pickup{gameState, position, gameState.getAssets().getBombPickupTexture()} {}

    void apply(Airplane::Airplane& airplane) noexcept override {
        if (airplane.tryAddBomb()) {
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomPowerUpSound());
            die();
        }
    };
};

#endif