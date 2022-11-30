/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "DeathEffect.h"

#include "Airplane/Airplane.h"
#include "Pickup.h"
#include "SoundEffect.h"
#include "AnimatedParticle.h"

#include <SFML/Audio.hpp>
using sf::Sound;

namespace Airplane {
    DeathEffect::DeathEffect(Airplane& owner, GameState& gameState) noexcept : 
        m_owner{owner}, m_gameState{gameState} {}

    ScoreDeathEffect::ScoreDeathEffect(Airplane& owner, GameState& gameState, int score) noexcept :
        DeathEffect(owner, gameState), m_score{score} {}

    void LootDeathEffect::handleDeath() noexcept {
        if (m_gameState.genRandom(std::uniform_real_distribution{0.0, 1.0}) < 0.1) {
            m_gameState.getEntities().addEntity(new HealthPickup{m_owner.getPosition(), m_gameState});
        }
    }

    void ExplosionDeathEffect::handleDeath() noexcept {
        m_gameState.getEntities().addEntity(new AnimatedParticle{m_owner.getPosition(), 
                                                m_gameState.getAssets().getExplosionAnimation(), 
                                                sf::seconds(0.1f), m_gameState});
        m_gameState.addSound(m_gameState.getAssets().getRandomExplosionSound(m_gameState.getRandomEngine()));
    }
}
