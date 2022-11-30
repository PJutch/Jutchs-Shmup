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

    void Manager::initGui() {
        sf::Vector2f screenSize = m_gameState.getScreenSize();
        auto& languageManager = m_gameState.getLanguageManager();

        sf::Vector2f menuSize{screenSize.y * 0.75f, screenSize.y * 0.75f};
        sf::Vector2f menuPos = (screenSize - menuSize) / 2.f;
        m_menu.setRect({menuPos.x, menuPos.y, menuSize.x, menuSize.y});
        m_menu.setColor(sf::Color{0, 0, 0, 128});
        m_menu.clearChildren();

        int characterSize = 100;
        const auto& font = m_gameState.getAssets().getFont();

        float buttonOutline = std::max(screenSize.y / 256.f, 0.5f);
        float buttonTextPadding = screenSize.y / 10.f;
        sf::Vector2f buttonSize{buttonTextPadding, screenSize.y / 10.f}; // .x will be updated
        sf::Color buttonColor{0, 0, 0, 128};
        sf::Color buttonElementColor = sf::Color::White;
        int buttonCharacterSize = 80;
        float buttonOffset = std::max(screenSize.y / 64.f, 1.f);

        float sliderHeight = std::max(screenSize.y / 64.f, 1.f);
        sf::Vector2f sliderRunnerSize{std::max(screenSize.y / 64.f, 1.f), 
                                      std::max(screenSize.y / 24.f, 1.f)};
        float sliderOffset = std::max(screenSize.y / 16.f, 1.f);
        float sliderLabelOffset = std::max(screenSize.y / 32.f, 1.f);
        int sliderCharacterSize = 50;

        sf::Color comboColor{0, 0, 0, 128};
        sf::Color comboElementColor = sf::Color::White;
        sf::Color comboSelectionColor{64, 96, 255};
        float comboOutline = -std::max(screenSize.y / 256.f, 0.5f);
        float comboTextPadding = screenSize.y / 10.f;
        sf::Vector2f comboSize{comboTextPadding, screenSize.y / 10.f}; // .x will be updated
        float comboOffset = std::max(screenSize.y / 16.f, 1.f);
        float comboLabelOffset = std::max(screenSize.y / 32.f, 1.f);
        int comboCharacterSize = 50;

        auto menuText = std::make_unique<Text>(languageManager.getMenuText(), 
                                                font, characterSize, sf::Color::White);
        menuText->setOrigin({menuText->getSize().x / 2.f, 0.f});
        menuText->setPosition({menuSize.x / 2.f, 0.f}); // .x at center, .y at top
        
        auto volumeText = std::make_unique<Text>(languageManager.getVolumeText(), 
                                                    font, sliderCharacterSize, sf::Color::White);

        // .x at center, .y under menuText
        volumeText->setOrigin({volumeText->getSize().x / 2.f, 0.f});
        volumeText->setPosition({menuSize.x / 2.f, menuText->getSize().y + sliderOffset});

        auto volumeSlider = std::make_unique<HorizontalSlider>([&gameState = m_gameState]() -> float {
            return gameState.getSounds().getVolume() / 100.f;
        }, [&gameState = m_gameState](float volume){
            gameState.getSounds().setVolume(volume * 100.f);
        }, sf::Color::White, sf::Color::Black);
        volumeSlider->setSize({2.f / 3.f * menuSize.x, sliderHeight});

        // .x at center, .y under volumeText
        volumeSlider->setOrigin({volumeSlider->getSize().x / 2, 0.f});
        volumeSlider->setPosition({menuSize.x / 2.f, 
            menuText->getSize().y + sliderOffset + volumeText->getSize().y + sliderLabelOffset});
        
        volumeSlider->setRunnerSize(sliderRunnerSize);

        // move center to center of the line
        volumeSlider->setRunnerOrigin({sliderRunnerSize.x / 2.f,
                                    sliderRunnerSize.y / 2.f - sliderHeight / 2.f});

        auto englishText = std::make_unique<Text>(
            languageManager.getLanguageName(LanguageManager::Language::ENGLISH), 
            font, comboCharacterSize, comboElementColor);
        englishText->setOrigin({englishText->getSize().x / 2.f, englishText->getSize().y * (2.f / 3.f)});
        englishText->setPosition({0.f, comboSize.y / 2.f}); // place in the center of the button
        comboSize.x = std::max(englishText->getSize().x + comboTextPadding, comboSize.x);

        auto russianText = std::make_unique<Text>(
            languageManager.getLanguageName(LanguageManager::Language::RUSSIAN), 
            font, comboCharacterSize, comboElementColor);
        russianText->setOrigin({russianText->getSize().x / 2.f, russianText->getSize().y * (2.f / 3.f)});
        russianText->setPosition({0.f, comboSize.y / 2.f}); // place in the center of the button
        comboSize.x = std::max(russianText->getSize().x + comboTextPadding, comboSize.x);

        auto languageCombo = std::make_unique<ComboBox>([&languageManager]() -> int {
            return static_cast<int>(languageManager.getLanguage());
        }, [&languageManager, this](int current){
            languageManager.setLanguage(static_cast<LanguageManager::Language>(current));
            initGui();
            throw Invalidated{"Gui reloaded"};
        }, comboColor, comboElementColor, comboOutline, comboSelectionColor);
        languageCombo->setSize(comboSize);
        languageCombo->addChild(std::move(englishText));
        languageCombo->addChild(std::move(russianText));

        // .x at center, .y under volumeSlider
        languageCombo->setOrigin({languageCombo->getSize().x / 2, 0.f});
        languageCombo->setPosition({menuSize.x / 2.f, 
            menuText->getSize().y + sliderOffset + volumeText->getSize().y 
            + sliderLabelOffset + sliderHeight + comboOffset});

        auto resumeText = std::make_unique<Text>(languageManager.getResumeText(), 
                                                 font, buttonCharacterSize, buttonElementColor);
        resumeText->setOrigin({resumeText->getSize().x / 2.f, resumeText->getSize().y});
        resumeText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
        buttonSize.x = std::max(resumeText->getSize().x + buttonTextPadding, buttonSize.x);

        auto exitText = std::make_unique<Text>(languageManager.getExitText(), 
                                               font, buttonCharacterSize, buttonElementColor);
        exitText->setOrigin({exitText->getSize().x / 2.f, exitText->getSize().y});
        exitText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
        buttonSize.x = std::max(exitText->getSize().x + buttonTextPadding, buttonSize.x);

        auto resumeButton = std::make_unique<Button>([this]{
            m_menuOpen = false;
        }, buttonColor, buttonElementColor, buttonOutline, buttonOutline);
        resumeButton->setSize(buttonSize);

        // .x at center, .y above exitButton
        resumeButton->setOrigin({buttonSize.x / 2.f, buttonSize.y});
        resumeButton->setPosition({menuSize.x / 2.f, 
            menuSize.y- 2 * buttonOffset - 3 * buttonOutline - buttonSize.y}); 
        resumeButton->setChild(std::move(resumeText));

        auto exitButton = std::make_unique<Button>([&gameState = m_gameState]{
            gameState.setShouldEnd(true);
        }, buttonColor, buttonElementColor, buttonOutline, buttonOutline);
        exitButton->setSize(buttonSize);

        // .x at center, .y at bottom
        exitButton->setOrigin({buttonSize.x / 2.f, buttonSize.y});
        exitButton->setPosition({menuSize.x / 2.f, menuSize.y - buttonOffset - buttonOutline});
        exitButton->setChild(std::move(exitText));

        m_menu.addChild(std::move(menuText));
        m_menu.addChild(std::move(volumeText));
        m_menu.addChild(std::move(volumeSlider));
        m_menu.addChild(std::move(resumeButton));
        m_menu.addChild(std::move(exitButton));

        m_menu.addChild(std::move(languageCombo)); // over buttons
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

    void Manager::drawNumber(int n, sf::Vector2f position, 
                             sf::RenderTarget& target, sf::RenderStates states) const {
        auto score = std::to_string(n);
        const auto& digitTextures = m_gameState.getAssets().getDigitTextures();
        auto digitSize = digitTextures[0].getSize();
        for (int i = 0; i < ssize(score); ++ i) {
            sf::Sprite digitSprite{digitTextures[score[i] - '0']};
            digitSprite.setPosition(i * digitSize.x + position.x, position.y);
            target.draw(digitSprite, states);
        }
    }

    void Manager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        const auto& assets = m_gameState.getAssets();

        auto healthSize = 2u * assets.getHealthTexture().getSize();
        sf::Sprite healthSprite{assets.getHealthTexture()};
        healthSprite.scale(2, 2);
        for (int i = 0; i < m_gameState.getPlayer().getHealth(); ++ i) {
            healthSprite.setPosition(i * healthSize.x, 0.f);
            target.draw(healthSprite, states);
        }

        drawNumber(m_gameState.getScore(), sf::Vector2f(0.f, healthSize.y), target, states);
        auto digitSize = assets.getDigitTextures()[0].getSize();
        const auto& scoreChanges = m_gameState.getScoreChanges();
        for (int i = 0; i < std::ssize(scoreChanges); ++ i) {
            sf::Sprite signSprite;
            if (scoreChanges[i].value >= 0) {
                signSprite.setTexture(assets.getPlusTexture());
            } else {
                signSprite.setTexture(assets.getMinusTexture());
            }
            signSprite.setPosition(0.f, healthSize.y + digitSize.y * (i + 1));
            target.draw(signSprite, states);

            drawNumber(scoreChanges[i].value, 
                    sf::Vector2f(digitSize.x, healthSize.y + digitSize.y * (i + 1)), target, states);
        }

        if (m_menuOpen) target.draw(m_menu, states);
    }
}
