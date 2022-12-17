/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_BOMB_COMPONENT_H_
#define AIRPLANE_BOMB_COMPONENT_H_

#include "GameState.h"
#include "Airplane/Flags.h"

#include <SFML/Window.hpp>
#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;

    class BombComponent {
    public:
        BombComponent(Airplane& owner, GameState& gameState) noexcept;
        virtual ~BombComponent() = default;

        virtual void handleEvent(sf::Event event) {}
        virtual void update(sf::Time elapsedTime) {}

        bool hasBomb() const noexcept {
            return m_hasBomb;
        }

        void setHasBomb(bool hasBomb) noexcept {
            m_hasBomb = hasBomb;
        }

        bool tryAddBomb() noexcept {
            if (m_hasBomb) return false;
            m_hasBomb = true;
            return true;
        }

        Flags getTextureFlags() const noexcept {
            return m_hasBomb ? Flags::HAS_BOMB : Flags::NO_BOMB;
        }
    protected:
        Airplane& m_owner;
        GameState& m_gameState;
        
        void tryBomb();
    private:
        bool m_hasBomb;
    };

    class EnemyBombComponent : public BombComponent {
    public:
        using BombComponent::BombComponent;

        void update(sf::Time elapsedTime) override;
    };

    class PlayerBombComponent : public BombComponent {
    public:
        using BombComponent::BombComponent;

        void handleEvent(sf::Event event) override {
            if (event.type == sf::Event::MouseButtonPressed 
             && event.mouseButton.button == sf::Mouse::Right) {
                tryBomb();
            }
        }
    };
}

#endif