/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "GameState.h"

#include <ranges>
#include <algorithm>
#include <utility>

GameState::GameState(sf::Vector2f screenSize) : 
        m_randomEngine{std::random_device{}()},
        m_assetManager{m_randomEngine}, m_entityManager{*this}, m_landManager{*this},
        m_screenSize{screenSize}, m_gameHeight{512},
        m_scoreManager{*this}, m_shouldResetAfter{sf::Time::Zero},
        m_shouldEnd{false}, m_guiManager{*this} {
    m_languageManager.setLanguage(LanguageManager::Language::ENGLISH);
    m_guiManager.initGui();    

    getEntities().init();
    m_landManager.init(); 
}

void GameState::handleEvent(const sf::Event& event) {
    m_guiManager.handleEvent(event);

    if (!m_guiManager.isMenuOpen())
        m_entityManager.handleEvent(event);   

    if (event.type == sf::Event::Closed) m_shouldEnd = true;
}

sf::Time MAX_LOADING_TICK = sf::seconds(0.015f);

void GameState::update() {
    sf::Time elapsedTime = m_tickClock.restart();

    m_soundManager.update();

    if (m_guiManager.isMenuOpen()) return;

    if (m_landManager.isLoading()) {
        while (m_landManager.isLoading() 
                && m_tickClock.getElapsedTime() < MAX_LOADING_TICK)
            m_landManager.load();

        return;
    }

    m_entityManager.update(elapsedTime);
    m_landManager.update();

    checkShouldReset(elapsedTime);
    
    m_scoreManager.update(elapsedTime);
}

void GameState::reset() {  
    m_clock.restart();
    m_scoreManager.reset();
    m_entityManager.reset();
    m_landManager.reset();
}

bool GameState::inActiveArea(float x) const noexcept {
    float playerX = getEntities().getPlayerPosition().x;
    return x + 2 * getGameHeight() >= playerX && x - 5 * getGameHeight() <= playerX;
}

void GameState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    auto prevView = target.getView();

    if (m_landManager.isLoading()) {
        m_guiManager.drawLoadingScreen(target, states);
        return;
    }

    target.setView(getView());
    target.draw(m_landManager, states);
    target.draw(m_entityManager, states);

    target.setView(prevView);
    target.draw(m_guiManager, states);
}

sf::View GameState::getView() const noexcept {
    float playerX = getEntities().getPlayerPosition().x;
    float aspectRatio = getScreenSize().x / getScreenSize().y;
    return sf::View{{playerX - getGameHeight() / 2.f, -getGameHeight() / 2.f, 
                     getGameHeight() * aspectRatio, getGameHeight()}};
}
