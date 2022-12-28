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

    int getScore() const noexcept {
        return m_score;
    }

    void addScore(int score) noexcept;

    void update();

    void reset();

    sf::Vector2f drawGui(sf::Vector2f position, sf::RenderTarget& target, sf::RenderStates states) const;
private:
    int m_score;

    struct ScoreChange {
        int value;
        sf::Time time;
    };
    std::deque<ScoreChange> m_scoreChanges;

    float m_scoredX;

    GameState& m_gameState;
};

#endif // SCORE_MANAGER_H_