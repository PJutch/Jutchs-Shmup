/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SCORE_MANAGER_H_
#define SCORE_MANAGER_H_

#include "declarations.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <deque>

class ScoreManager {
public:
    ScoreManager(GameState& gameState) noexcept;

    ~ScoreManager() {
        saveBestScore();
    }

    // score may be negative
    void addScore(float score) noexcept;

    void update(sf::Time elapsedTime);

    void reset();

    sf::Vector2f drawGui(sf::Vector2f position, 
        sf::RenderTarget& target, sf::RenderStates states) const;
private:
    float m_score;
    float m_bestScore;

    float m_scoreChange;
    float m_changeApplySpeed;
    sf::Time m_changeApplyStart;

    float m_scoredX;

    GameState& m_gameState;

    void saveBestScore() noexcept;
};

#endif // SCORE_MANAGER_H_