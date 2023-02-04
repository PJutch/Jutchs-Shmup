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

#include <fstream>
#include <utility>

const sf::Time SCORE_CHANGE_APPLY_DELAY = sf::seconds(0.5f);
const sf::Time SCORE_CHANGE_APPLY_TIME = sf::seconds(0.5f);

ScoreManager::ScoreManager(GameState& gameState) noexcept : 
        m_score{0}, m_scoredX{0.f}, m_gameState{gameState}, m_scoreChange{0}, 
        m_changeApplySpeed{0.0f}, m_changeApplyStart{sf::Time::Zero} {
    std::ifstream best_score_file{"best_score.txt"};
    if (best_score_file)
        best_score_file >> m_bestScore;
    else
        m_bestScore = 0.f;
}

void ScoreManager::addScore(float score) noexcept {
    m_scoreChange += score;

    m_changeApplySpeed = m_scoreChange / SCORE_CHANGE_APPLY_TIME.asSeconds();
    m_changeApplyStart = m_gameState.getCurrentTime() + SCORE_CHANGE_APPLY_DELAY;
}

void ScoreManager::update(sf::Time elapsedTime) {
    float playerX = m_gameState.getEntities().getPlayerPosition().x;
    while (playerX > m_scoredX) {
        m_scoredX += 32;
        m_score += 1;
    }

    auto currentTime = m_gameState.getCurrentTime();
    if (currentTime > m_changeApplyStart) {
        sf::Time applyTime = std::min(elapsedTime, currentTime - m_changeApplyStart);
        float applyedScore = std::min(m_changeApplySpeed * applyTime.asSeconds(), m_scoreChange);
        
        m_score += applyedScore;
        m_scoreChange -= applyedScore;
    }
}

void ScoreManager::saveBestScore() noexcept {
    m_bestScore = std::max(m_score + m_scoreChange, m_bestScore);

    std::ofstream best_score_file{"best_score.txt"};
    best_score_file << m_bestScore << '\n';
}

void ScoreManager::reset() {
    saveBestScore();

    m_score = 0;
    m_scoreChange = 0;
    m_changeApplySpeed = 0.f;
    m_scoredX = 0.f;
}

sf::Vector2f ScoreManager::drawGui(sf::Vector2f position, 
        sf::RenderTarget& target, sf::RenderStates states) const {
    const auto& assets = m_gameState.getAssets();
    auto digitSize = assets.getDigitTextures()[0].getSize();

    float width = Gui::drawNumber(static_cast<int>(m_score), position, target, states, assets).x;

    const sf::Texture& slashTexture = assets.getSlashTexture();
    sf::Vector2u slashSize = slashTexture.getSize();

    sf::Sprite slashSprite{slashTexture};
    slashSprite.setPosition(position.x + width, position.y);
    target.draw(slashSprite, states);

    width += slashSize.x;

    width += Gui::drawNumber(static_cast<int>(m_bestScore), {position.x + width, position.y}, 
                             target, states, assets).x;

    if (m_scoreChange != 0) {
        sf::Vector2f scoreChangePosition{position.x, position.y + digitSize.y};
        
        width = std::max(Gui::drawSignedNumber(static_cast<int>(m_scoreChange), 
            scoreChangePosition, target, states, assets).x, width);
    }

    return sf::Vector2f(width, 2 * digitSize.y);
}
