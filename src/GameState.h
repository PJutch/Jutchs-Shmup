/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef GAME_STATE_H_
#define GAME_STATE_H_

#include "Entity.h"
#include "AssetManager.h"
#include "SoundEffect.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <random>
#include <vector>
#include <array>
#include <concepts>
#include <memory>
#include <deque>

class Airplane;

class GameState : public sf::Drawable {
public:
    GameState(sf::Vector2f screenSize);

    const AssetManager& getAssets() const noexcept {
        return m_assetManager;
    }

    template<std::invocable<std::mt19937_64&> Distribution>
    auto genRandom(Distribution distibution) noexcept {
        return distibution(m_randomEngine);
    }

    std::mt19937_64& getRandomEngine() noexcept {
        return m_randomEngine;
    }

    float getGameHeight() const noexcept {
        return m_gameHeight;
    }

    const EntityManager& getEntities() const noexcept {
        return m_entityManager;
    }

    EntityManager& getEntities() noexcept {
        return m_entityManager;
    }

    const Airplane& getPlayer() const noexcept {
        return *m_player;
    }

    Airplane& getPlayer() noexcept {
        return *m_player;
    }

    void addSound(SoundEffect* sound) noexcept {
        m_sounds.emplace_back(sound);
    }

    void addScore(int score) noexcept {
        m_scoreChanges.emplace_back(score, m_clock.getElapsedTime());
    }

    sf::Vector2f getScreenSize() const noexcept {
        return m_screenSize;
    }

    bool inActiveArea(float x) const noexcept;

    void setShouldResetAfter(sf::Time time) noexcept {
        m_shouldResetAfter = time;
    }

    void handleEvent(sf::Event event) noexcept {
        m_entityManager.handleEvent(event);
    }

    void update() noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    AssetManager m_assetManager;

    EntityManager m_entityManager;

    sf::Clock m_tickClock;
    sf::Clock m_clock;

    std::mt19937_64 m_randomEngine;

    Airplane* m_player;

    int m_score;
    struct ScoreChange {
        int value;
        sf::Time time;
    };
    std::deque<ScoreChange> m_scoreChanges;

    sf::Time m_shouldResetAfter;

    sf::Vector2f m_screenSize;
    float m_gameHeight;
    float m_spawnX;

    std::vector<std::unique_ptr<SoundEffect>> m_sounds;

    sf::View getView() const noexcept;

    void reset() noexcept;

    void trySpawnEnemy(sf::Vector2f position) noexcept;

    void drawNumber(int n, sf::Vector2f position, 
                    sf::RenderTarget& target, sf::RenderStates states) const noexcept;
};

#endif