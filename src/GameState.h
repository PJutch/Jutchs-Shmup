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

#include "AssetManager.h"
#include "EntityManager.h"
#include "Gui/Panel.h"
#include "LanguageManager.h"
#include "Gui/Manager.h"
#include "SoundManager.h"
#include "LandManager.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <random>
#include <vector>
#include <array>
#include <concepts>
#include <memory>
#include <deque>

namespace Airplane {
    class Airplane;
}

class GameState : public sf::Drawable {
public:
    GameState(sf::Vector2f screenSize);

    GameState(const GameState&) noexcept = delete;
    GameState& operator=(const GameState&) noexcept = delete;
    GameState(GameState&&) noexcept = delete;
    GameState& operator=(GameState&&) noexcept = delete;

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

    const LandManager& getLand() const noexcept {
        return m_landManager;
    }

    LandManager& getLand() noexcept {
        return m_landManager;
    }

    const Airplane::Airplane& getPlayer() const noexcept {
        return *m_player;
    }

    Airplane::Airplane& getPlayer() noexcept {
        return *m_player;
    }

    void addScore(int score) noexcept {
        m_score += score;
        m_scoreChanges.emplace_back(score, m_clock.getElapsedTime());
    }

    const LanguageManager& getLanguageManager() const noexcept {
        return m_languageManager;
    }

    LanguageManager& getLanguageManager() noexcept {
        return m_languageManager;
    }

    const SoundManager& getSounds() const noexcept {
        return m_soundManager;
    }

    SoundManager& getSounds() noexcept {
        return m_soundManager;
    }

    sf::Vector2f getScreenSize() const noexcept {
        return m_screenSize;
    }

    bool inActiveArea(float x) const noexcept;

    void setShouldResetAfter(sf::Time time) noexcept {
        m_shouldResetAfter = time;
    }

    bool shouldEnd() const noexcept {
        return m_shouldEnd;
    }

    void setShouldEnd(bool shouldEnd) noexcept {
        m_shouldEnd = shouldEnd;
    }

    sf::Time getCurrentTime() const noexcept {
        return m_clock.getElapsedTime();
    }

    int getScore() const noexcept {
        return m_score;
    }

    struct ScoreChange {
        int value;
        sf::Time time;
    };

    const std::deque<ScoreChange>& getScoreChanges() const noexcept {
        return m_scoreChanges;
    }

    float getScoreChangeAlpha(int index) const noexcept {
        return 1.f - (m_clock.getElapsedTime() - m_scoreChanges[index].time) / sf::seconds(0.5f);
    }

    void handleEvent(const sf::Event& event);

    void update();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
    std::mt19937_64 m_randomEngine;

    AssetManager m_assetManager;

    EntityManager m_entityManager;

    LandManager m_landManager;

    SoundManager m_soundManager;

    LanguageManager m_languageManager;

    Gui::Manager m_guiManager;

    sf::Clock m_tickClock;
    sf::Clock m_clock;

    Airplane::Airplane* m_player;

    int m_score;
    std::deque<ScoreChange> m_scoreChanges;

    sf::Time m_shouldResetAfter;

    sf::Vector2f m_screenSize;
    float m_gameHeight;
    float m_spawnX;

    bool m_shouldEnd;

    void initPlayer();

    sf::View getView() const noexcept;

    void reset();

    void checkReset(sf::Time elapsedTime) {
        if (m_shouldResetAfter > sf::Time::Zero) {
            m_shouldResetAfter -= elapsedTime;
            if (m_shouldResetAfter <= sf::Time::Zero) reset();
        }
    }

    void checkEnemySpawn();

    void updateScore();

    void trySpawnEnemy(sf::Vector2f position);
};

#endif