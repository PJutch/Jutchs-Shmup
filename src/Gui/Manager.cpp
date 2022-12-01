/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Gui/Manager.h"
#include "Gui/Button.h"
#include "Gui/ComboBox.h"
#include "Gui/HorizontalSlider.h"
#include "Gui/Panel.h"
#include "Gui/Text.h"

#include "GameState.h"
#include "LanguageManager.h"
#include "Airplane/Airplane.h"

#include <algorithm>
#include <exception>

namespace Gui {
    class Invalidated : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    Manager::Manager(GameState& gameState) : m_gameState{gameState}, m_menuOpen{false} {}

    sf::Vector2f Manager::addMenuText(sf::Vector2f position) {
        auto menuText = std::make_unique<Text>(m_gameState.getLanguageManager().getMenuText(), 
            m_gameState.getAssets().getFont(), 100, sf::Color::White);
        
        auto size = menuText->getSize();
        menuText->setOrigin({size.x / 2.f, 0.f});
        menuText->setPosition(position);

        m_menu.addChild(std::move(menuText));
        return size;
    }

    sf::Vector2f Manager::addVolumeSlider(sf::Vector2f position) {
        sf::Vector2f screenSize = m_gameState.getScreenSize();

        auto volumeText = std::make_unique<Text>(m_gameState.getLanguageManager().getVolumeText(), 
            m_gameState.getAssets().getFont(), 50, sf::Color::White);
        
        auto textSize = volumeText->getSize();
        volumeText->setOrigin({textSize.x / 2.f, 0.f});
        volumeText->setPosition(position);

        position.y += textSize.y;
        m_menu.addChild(std::move(volumeText));

        float sliderTextOffset = std::max(screenSize.y / 32.f, 1.f);
        position.y += sliderTextOffset;

        auto volumeSlider = std::make_unique<HorizontalSlider>(
           [&sounds = m_gameState.getSounds()]() -> float {
            return sounds.getVolume();
        }, [&sounds = m_gameState.getSounds()](float volume){
            sounds.setVolume(volume);
        }, sf::Color::White, sf::Color::Black);

        sf::Vector2f sliderSize{2.f / 3.f * m_menu.getSize().x, std::max(screenSize.y / 64.f, 1.f)};
        volumeSlider->setSize(sliderSize);
        volumeSlider->setOrigin({sliderSize.x / 2, 0.f});
        volumeSlider->setPosition(position); // under the volumeText
        
        sf::Vector2f sliderRunnerSize{std::max(screenSize.y / 64.f, 1.f), 
                                      std::max(screenSize.y / 24.f, 1.f)};
        volumeSlider->setRunnerSize(sliderRunnerSize);

        // move the center to the center of the line
        volumeSlider->setRunnerOrigin({sliderRunnerSize.x / 2.f, 
                                       sliderRunnerSize.y / 2.f - sliderSize.y / 2.f});

        m_menu.addChild(std::move(volumeSlider));
        return {std::max(textSize.x, sliderSize.x), textSize.y + sliderTextOffset + sliderSize.y};
    }

    sf::Vector2f Manager::addLanguageCombo(sf::Vector2f position) {
        sf::Vector2f screenSize = m_gameState.getScreenSize();
        const auto& font = m_gameState.getAssets().getFont();
        auto& languageManager = m_gameState.getLanguageManager();

        float comboTextPadding = screenSize.y / 10.f;
        sf::Vector2f comboSize{comboTextPadding, comboTextPadding}; // .x will be updated

        auto englishText = std::make_unique<Text>(
            languageManager.getLanguageName(LanguageManager::Language::ENGLISH), 
            font, 50, sf::Color::White);
        englishText->setOrigin({englishText->getSize().x / 2.f, englishText->getSize().y * (2.f / 3.f)});
        englishText->setPosition({0.f, comboSize.y / 2.f}); // at the center of the combo
        comboSize.x = std::max(englishText->getSize().x + comboTextPadding, comboSize.x);

        auto russianText = std::make_unique<Text>(
            languageManager.getLanguageName(LanguageManager::Language::RUSSIAN), 
            font, 50, sf::Color::White);
        russianText->setOrigin({russianText->getSize().x / 2.f, russianText->getSize().y * (2.f / 3.f)});
        russianText->setPosition({0.f, comboSize.y / 2.f}); // at the center of the combo
        comboSize.x = std::max(russianText->getSize().x + comboTextPadding, comboSize.x);

        auto languageCombo = std::make_unique<ComboBox>([&languageManager]() -> int {
            return static_cast<int>(languageManager.getLanguage());
        }, [&languageManager, this](int current){
            languageManager.setLanguage(static_cast<LanguageManager::Language>(current));
            initGui();
            throw Invalidated{"Gui reloaded"};
        }, sf::Color{0, 0, 0, 128}, sf::Color::White, 
            -std::max(screenSize.y / 256.f, 0.5f), sf::Color{64, 96, 255});
        languageCombo->setSize(comboSize);
        languageCombo->addChild(std::move(englishText));
        languageCombo->addChild(std::move(russianText));

        languageCombo->setOrigin({comboSize.x / 2, 0.f});
        languageCombo->setPosition(position);

        m_menu.addChild(std::move(languageCombo));
        return comboSize;
    }

    sf::Vector2f Manager::addMenuButtons(sf::Vector2f position) {
        sf::Vector2f screenSize = m_gameState.getScreenSize();
        const auto& font = m_gameState.getAssets().getFont();
        auto& languageManager = m_gameState.getLanguageManager();

        float buttonTextPadding = screenSize.y / 10.f;
        sf::Vector2f buttonSize{buttonTextPadding, buttonTextPadding}; // .x will be updated

        auto resumeText = std::make_unique<Text>(languageManager.getResumeText(), 
                                                 font, 80, sf::Color::White);
        resumeText->setOrigin({resumeText->getSize().x / 2.f, resumeText->getSize().y});
        resumeText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
        buttonSize.x = std::max(resumeText->getSize().x + buttonTextPadding, buttonSize.x);

        auto exitText = std::make_unique<Text>(languageManager.getExitText(), 
                                               font, 80, sf::Color::White);
        exitText->setOrigin({exitText->getSize().x / 2.f, exitText->getSize().y});
        exitText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
        buttonSize.x = std::max(exitText->getSize().x + buttonTextPadding, buttonSize.x);

        float buttonOutline = std::max(screenSize.y / 256.f, 0.5f);
        float buttonOffset = std::max(screenSize.y / 64.f, 1.f);

        sf::Vector2f totalSize{buttonSize.x, 0.f};

        totalSize.y += buttonOffset + buttonOutline;

        auto exitButton = std::make_unique<Button>([&gameState = m_gameState]{
            gameState.setShouldEnd(true);
        }, sf::Color{0, 0, 0, 128}, sf::Color::White, buttonOutline, buttonOutline);
        exitButton->setSize(buttonSize);

        exitButton->setOrigin({buttonSize.x / 2.f, buttonSize.y});
        exitButton->setPosition({position.x, position.y - totalSize.y});
        exitButton->setChild(std::move(exitText));

        totalSize.y += buttonSize.y;
        m_menu.addChild(std::move(exitButton));

        totalSize.y += buttonOutline + buttonOffset + buttonOutline;

        auto resumeButton = std::make_unique<Button>([this]{
            m_menuOpen = false;
        }, sf::Color{0, 0, 0, 128}, sf::Color::White, buttonOutline, buttonOutline);
        resumeButton->setSize(buttonSize);

        // .x at center, .y above exitButton
        resumeButton->setOrigin({buttonSize.x / 2.f, buttonSize.y});
        resumeButton->setPosition({position.x, position.y - totalSize.y});
        resumeButton->setChild(std::move(resumeText));

        totalSize.y += buttonSize.y;
        m_menu.addChild(std::move(resumeButton));

        return totalSize;
    }

    void Manager::initGui() {
        sf::Vector2f screenSize = m_gameState.getScreenSize();

        sf::Vector2f menuSize{screenSize.y * 0.75f, screenSize.y * 0.75f};
        sf::Vector2f menuPos = (screenSize - menuSize) / 2.f;
        m_menu.setRect({menuPos.x, menuPos.y, menuSize.x, menuSize.y});
        m_menu.setColor(sf::Color{0, 0, 0, 128});
        m_menu.clearChildren();

        sf::Vector2f topCenterPos{m_menu.getSize().x / 2.f, 0.f};

        topCenterPos.y += addMenuText(topCenterPos).y; // at top center

        topCenterPos.y += std::max(screenSize.y / 16.f, 1.f); // at the center under the menuText
        topCenterPos.y += addVolumeSlider(topCenterPos).y; 

        addMenuButtons({menuSize.x / 2.f, menuSize.y}); // at center bottom

        topCenterPos.y += std::max(screenSize.y / 16.f, 1.f); // at center under the volumeSlider   
        addLanguageCombo(topCenterPos); // over the buttons
    }

    void Manager::handleEvent(const sf::Event& event) {
        if (m_menuOpen) {
            try {
                m_menu.handleEvent(event);
            } catch (const Invalidated&) {}
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            m_menuOpen = !m_menuOpen;
    }

    sf::Vector2f Manager::drawNumber(int n, sf::Vector2f position, float alpha,
                             sf::RenderTarget& target, sf::RenderStates states) const {
        auto score = std::to_string(n);
        const auto& digitTextures = m_gameState.getAssets().getDigitTextures();
        auto digitSize = digitTextures[0].getSize();

        for (int i = 0; i < std::ssize(score); ++ i) {
            sf::Sprite digitSprite{digitTextures[score[i] - '0']};
            digitSprite.setColor(sf::Color(255, 255, 255, alpha * 255));
            digitSprite.setPosition(i * digitSize.x + position.x, position.y);
            target.draw(digitSprite, states);
        }

        return sf::Vector2f(std::ssize(score) * digitSize.x, digitSize.y);
    }

    sf::Vector2f Manager::drawHealth(sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states) const {
        int health = m_gameState.getPlayer().getHealth();

        const auto& assets = m_gameState.getAssets();
        auto healthSize = 2u * assets.getHealthTexture().getSize();
        sf::Sprite healthSprite{assets.getHealthTexture()};
        healthSprite.scale(2, 2);

        for (int i = 0; i < health; ++ i) {
            healthSprite.setPosition(position.x + i * healthSize.x, position.y);
            target.draw(healthSprite, states);
        }

        return sf::Vector2f(health * healthSize.x, healthSize.y);
    }

    sf::Vector2f Manager::drawScore(sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states) const {
        auto maxWidth = drawNumber(m_gameState.getScore(), position, target, states).x;

        const auto& assets = m_gameState.getAssets();
        auto digitSize = assets.getDigitTextures()[0].getSize();
        const auto& scoreChanges = m_gameState.getScoreChanges();

        for (int i = std::ssize(scoreChanges) - 1; i >= 0; -- i) {
            float alpha = m_gameState.getScoreChangeAlpha(i);
            if (alpha <= 0) continue;

            float yPos = position.y + digitSize.y * (std::ssize(scoreChanges) - i);

            sf::Sprite signSprite;
            signSprite.setColor(sf::Color(255, 255, 255, alpha * 255));
            if (scoreChanges[i].value >= 0) {
                signSprite.setTexture(assets.getPlusTexture());
            } else {
                signSprite.setTexture(assets.getMinusTexture());
            }
            signSprite.setPosition(position.x, yPos);
            target.draw(signSprite, states);

            maxWidth = std::max(maxWidth,
                drawNumber(scoreChanges[i].value, 
                    sf::Vector2f(position.x + digitSize.x, yPos), 
                    alpha, target, states).x);
        }

        return sf::Vector2f(maxWidth, (std::ssize(scoreChanges) + 1) * digitSize.y);
    }

    void Manager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        auto healthSize = drawHealth({0.f, 0.f}, target, states);
        drawScore({0.f, healthSize.y}, target, states);

        if (m_menuOpen) target.draw(m_menu, states);
    }
}
