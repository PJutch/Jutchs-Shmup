/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ScoreManager.h"

#include "GameState.h"

#include "Airplane/Airplane.h"

#include "Gui/drawNumber.h"

const sf::Time SCORE_CHANGE_SHOW_TIME = sf::seconds(0.5f);

ScoreManager::ScoreManager(GameState& gameState) noexcept : 
    m_score{0}, m_scoredX{0.f}, m_gameState{gameState} {}

void ScoreManager::addScore(int score) noexcept {
    m_score += score;
    m_scoreChanges.emplace_back(score, m_gameState.getCurrentTime());
}

void ScoreManager::update() {
    while (m_gameState.getPlayer().getPosition().x > m_scoredX) {
        m_scoredX += 32;
        addScore(1);
    }

    while (!m_scoreChanges.empty() 
            && m_gameState.getCurrentTime() - m_scoreChanges[0].time >= SCORE_CHANGE_SHOW_TIME) {
        m_scoreChanges.pop_front();
    }
}

void ScoreManager::reset() {
    m_score = 0;
    m_scoreChanges.clear();
    m_scoredX = 0.f;
}

sf::Vector2f ScoreManager::drawGui(sf::Vector2f position, 
        sf::RenderTarget& target, sf::RenderStates states) const {
    const auto& assets = m_gameState.getAssets();
    auto digitSize = assets.getDigitTextures()[0].getSize();

    float maxWidth = Gui::drawNumber(m_score, position, target, states, assets).x;

    for (int i = std::ssize(m_scoreChanges) - 1; i >= 0; -- i) {
        float alpha = 1.f - (m_gameState.getCurrentTime() - m_scoreChanges[i].time) / SCORE_CHANGE_SHOW_TIME;
        if (alpha <= 0) continue;

        sf::Vector2f scoreChangePosition{0.f, position.y + digitSize.y * (std::ssize(m_scoreChanges) - i)};
        
        maxWidth = std::max(maxWidth, Gui::drawSignedNumber(m_scoreChanges[i].value, 
            scoreChangePosition, alpha, target, states, assets).x);
    }

    return sf::Vector2f(maxWidth, (std::ssize(m_scoreChanges) + 1) * digitSize.y);
}
