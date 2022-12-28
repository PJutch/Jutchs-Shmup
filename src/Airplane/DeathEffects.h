/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_DEATH_EFFECTS_H_
#define AIRPLANE_DEATH_EFFECTS_H_

#include "DeathEffect.h"

#include "Airplane.h"

#include "../GameState.h"
#include "../Pickup.h"

#include <SFML/System.hpp>

namespace Airplane {
    class Airplane;

    class LoseDeathEffect : public DeathEffect {
    public:
        LoseDeathEffect(GameState& gameState) noexcept : m_gameState{gameState} {}
        LoseDeathEffect(Airplane&, GameState& gameState) noexcept : LoseDeathEffect(gameState) {}

        void handleDeath() noexcept override {
            m_gameState.setShouldResetAfter(sf::seconds(1.0f));
        }
    private:
        GameState& m_gameState;
    };

    class ScoreDeathEffect : public DeathEffect {
    public:
        ScoreDeathEffect(GameState& gameState, int score) noexcept : 
            m_gameState{gameState}, m_score{score} {}
        ScoreDeathEffect(Airplane&, GameState& gameState, int score) noexcept : 
            ScoreDeathEffect(gameState, score) {}

        void handleDeath() noexcept override {
            m_gameState.getScoreManager().addScore(m_score);
        }
    private:
        GameState& m_gameState;
        int m_score;
    };

    class LootDeathEffect : public DeathEffect {
    public:
        LootDeathEffect(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        void handleDeath() noexcept override {
            if (m_owner.hasBomb()) {
                m_gameState.getEntities().addEntity<BombPickup>(m_owner.getPosition());
            } else {
                if (std::uniform_real_distribution{0.0, 1.0}(m_gameState.getRandomEngine()) < 0.1) {
                    m_gameState.getEntities().addEntity<HealthPickup>(m_owner.getPosition());
                }
            }
        }
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };

    class ExplosionDeathEffect : public DeathEffect {
    public:
        ExplosionDeathEffect(Airplane& owner, GameState& gameState) noexcept : 
            m_owner{owner}, m_gameState{gameState} {}

        void handleDeath() noexcept override {
            m_gameState.getEntities().addEntity<AnimatedParticle>(m_owner.getPosition(), 
                m_gameState.getAssets().getExplosionAnimation(), sf::seconds(0.1f));
            
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomExplosionSound());
        }
    private:
        Airplane& m_owner;
        GameState& m_gameState;
    };
}

#endif