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

#include "Gui/Panel.h"
#include "Gui/Manager.h"

#include "EntityManager.h"
#include "LandManager.h"
#include "ScoreManager.h"

#include "AssetManager.h"
#include "LanguageManager.h"
#include "SoundManager.h"

#include "Timer.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <random>
#include <vector>
#include <array>
#include <concepts>
#include <memory>
#include <deque>

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

    const ScoreManager& getScoreManager() const noexcept {
        return m_scoreManager;
    }

    ScoreManager& getScoreManager() noexcept {
        return m_scoreManager;
    }

    sf::Vector2f getScreenSize() const noexcept {
        return m_screenSize;
    }

    bool inActiveArea(float x) const noexcept;

    void setShouldResetAfter(sf::Time time) noexcept {
        m_resetTimer.start(time);
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

    void handleEvent(const sf::Event& event);

    void update();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    bool isLoading() noexcept {
        return m_landManager.isLoading();
    }
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

    ScoreManager m_scoreManager;

    OnceTimer m_resetTimer;

    sf::Vector2f m_screenSize;
    float m_gameHeight;

    bool m_shouldEnd;

    sf::View getView() const noexcept;

    void reset();

    void checkShouldReset(sf::Time elapsedTime) {
        if (!m_resetTimer.isFinished()) {
            m_resetTimer.update(elapsedTime);
            if (m_resetTimer.isFinished()) 
                reset();
        }
    }
};

#endif