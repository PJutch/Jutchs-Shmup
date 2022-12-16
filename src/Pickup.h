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

class Pickup : public Sprite<Pickup> {
public:
    Pickup(sf::Vector2f position, const sf::Texture& texture, GameState& gameState) noexcept: 
            Sprite{gameState}, m_alive{true} {
        setTexture(texture);

        auto size = texture.getSize();
        setOrigin(size.x / 2.f, size.y / 2.f);

        setPosition(position);
    }

    virtual ~Pickup() = default;

    void update(sf::Time) noexcept override {}

    void acceptCollide(Airplane::Airplane& other) noexcept override {
        if (other.canUsePickups()) apply(other);
    }

    virtual void apply(Airplane::Airplane& airplane) noexcept = 0;

    bool shouldBeDeleted() const noexcept override {
        return !(m_alive && m_gameState.inActiveArea(getPosition().x));
    }

    bool isPassable() const noexcept override {
        return true;
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
    HealthPickup(sf::Vector2f position, GameState& gameState) noexcept: 
        Pickup{position, gameState.getAssets().getHealthPickupTexture(), gameState} {}

    void apply(Airplane::Airplane& airplane) noexcept override {
        if (isActive() && airplane.tryAddHealth(1)) {
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomPowerUpSound());
            die();
        }
    };
};

class BombPickup : public Pickup {
public:
    BombPickup(sf::Vector2f position, GameState& gameState) noexcept: 
        Pickup{position, gameState.getAssets().getBombPickupTexture(), gameState} {}

    void apply(Airplane::Airplane& airplane) noexcept override {
        if (isActive() && airplane.tryAddBomb()) {
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomPowerUpSound());
            die();
        }
    };
};

#endif