/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_DEATH_EFFECT_H_
#define AIRPLANE_DEATH_EFFECT_H_

#include "GameState.h"
#include "AnimatedParticle.h"

#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;

    class DeathEffect {
    public:
        DeathEffect(Airplane& owner, GameState& gameState) noexcept;
        virtual ~DeathEffect() = default;

        virtual void handleDeath() noexcept = 0;
    protected:
        Airplane& m_owner;
        GameState& m_gameState;
    };

    class LoseDeathEffect : public DeathEffect {
    public:
        using DeathEffect::DeathEffect;

        void handleDeath() noexcept override {
            m_gameState.setShouldResetAfter(sf::seconds(1.0f));
        }
    };

    class ScoreDeathEffect : public DeathEffect {
    public:
        ScoreDeathEffect(Airplane& owner, GameState& gameState, int score) noexcept;

        void handleDeath() noexcept override {
            m_gameState.addScore(m_score);
        }
    private:
        int m_score;
    };

    class LootDeathEffect : public DeathEffect {
    public:
        using DeathEffect::DeathEffect;

        void handleDeath() noexcept override;
    };

    class ExplosionDeathEffect : public DeathEffect {
    public:
        using DeathEffect::DeathEffect;

        void handleDeath() noexcept override;
    };
}

#endif