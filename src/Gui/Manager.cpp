/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Manager.h"
#include "Elements.h"

#include "../GameState.h"
#include "../LanguageManager.h"
#include "../Airplane/Airplane.h"

#include <string>
#include <algorithm>
#include <exception>

namespace Gui {
    class Invalidated : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    Manager::Manager(GameState& gameState) : 
        m_gameState{gameState}, m_menuOpen{false}, 
        m_loadingDots{0}, m_loadingDotsChangeDelay{LOADING_DOTS_CHANGE_DELAY} {}

    sf::Vector2f Manager::addMenuText(sf::Vector2f position) {
        auto menuText = std::make_unique<Text>();
        menuText->setString(m_gameState.getLanguageManager().getMenuText());
        menuText->setStyle(m_gameState.getAssets().getFont(), 100, sf::Color::White);
        
        auto size = menuText->getSize();
        menuText->setOrigin({size.x / 2.f, 0.f});
        menuText->setPosition(position);

        m_menu.addChild(std::move(menuText));
        return size;
    }

    sf::Vector2f Manager::addVolumeSlider(sf::Vector2f position) {
        sf::Vector2f screenSize = m_gameState.getScreenSize();

        auto volumeText = std::make_unique<Text>();
        volumeText->setString(m_gameState.getLanguageManager().getVolumeText());
        volumeText->setStyle(m_gameState.getAssets().getFont(), 50, sf::Color::White);
        
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

        auto englishText = std::make_unique<Text>();
        englishText->setString(languageManager.getLanguageName(LanguageManager::Language::ENGLISH));
        englishText->setStyle(font, 50, sf::Color::White);
        englishText->setOrigin({englishText->getSize().x / 2.f, englishText->getSize().y * (2.f / 3.f)});
        englishText->setPosition({0.f, comboSize.y / 2.f}); // at the center of the combo
        comboSize.x = std::max(englishText->getSize().x + comboTextPadding, comboSize.x);

        auto russianText = std::make_unique<Text>();
        russianText->setString(languageManager.getLanguageName(LanguageManager::Language::RUSSIAN));
        russianText->setStyle(font, 50, sf::Color::White);
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

        auto resumeText = std::make_unique<Text>();
        resumeText->setString(languageManager.getResumeText());
        resumeText->setStyle(font, 80, sf::Color::White);
        resumeText->setOrigin({resumeText->getSize().x / 2.f, resumeText->getSize().y});
        resumeText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
        buttonSize.x = std::max(resumeText->getSize().x + buttonTextPadding, buttonSize.x);

        auto exitText = std::make_unique<Text>();
        exitText->setString(languageManager.getExitText());
        exitText->setStyle(font, 80, sf::Color::White);
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

    sf::Vector2f Manager::createLoadingText(sf::Vector2f position) {
        m_loadingText.setString(m_gameState.getLanguageManager().getLoadingText());
        m_loadingText.setStyle(m_gameState.getAssets().getFont(), 80, sf::Color::White);

        m_loadingText.setOrigin(m_loadingText.getSize() / 2.f);
        m_loadingText.setPosition(position);

        return m_loadingText.getSize();
    }

    sf::Vector2f Manager::createBestScoreText(sf::Vector2f position) {
        m_bestScoreText.setStyle(m_gameState.getAssets().getFont(), 80, sf::Color::White);
        m_bestScoreText.setPosition(position);

        setBestScoreText(m_gameState.getLanguageManager().getBestScoreText());

        return m_bestScoreText.getSize();
    }

    void Manager::setBestScoreText(const std::string& string) {
        m_bestScoreText.setString(string);
        m_bestScoreText.setOrigin({m_bestScoreText.getSize().x / 2.f, 0.f});
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

        topCenterPos.y += std::max(screenSize.y / 16.f, 1.f); // at the center under the volumeSlider   
        addLanguageCombo(topCenterPos); // over the buttons

        // at the center of the screen
        sf::Vector2f loadingTextSize = createLoadingText(screenSize / 2.f); 
        createBestScoreText({screenSize.x / 2.f, screenSize.y / 2.f + loadingTextSize.y / 2.f + 10.f});
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

    void Manager::update(sf::Time elapsedTime) {
        if (m_gameState.isLoading()) {
            m_loadingDotsChangeDelay -= elapsedTime;
            if (m_loadingDotsChangeDelay <= sf::Time::Zero) {
                if (++ m_loadingDots > 3)
                    m_loadingDots = 0;

                std::string newLoadingText = m_gameState.getLanguageManager().getLoadingText();
                for (int i = 0; i < m_loadingDots; ++ i)
                    newLoadingText.push_back('.');
                m_loadingText.setString(newLoadingText);

                m_loadingDotsChangeDelay = LOADING_DOTS_CHANGE_DELAY;
            }

            std::string bestScoreString = m_gameState.getLanguageManager().getBestScoreText()
                + ' ' + std::to_string(static_cast<int>(m_gameState.getScoreManager().getBestScore()));
            setBestScoreText(bestScoreString);
        }
    }

    sf::Vector2f Manager::drawHealth(sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states) const {
        int health = m_gameState.getEntities().getPlayerHealth();

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

    void Manager::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        auto healthSize = drawHealth({0.f, 0.f}, target, states);
        m_gameState.getScoreManager().drawGui({0.f, healthSize.y}, target, states);

        if (m_menuOpen) target.draw(m_menu, states);
    }

    void Manager::drawLoadingScreen(sf::RenderTarget& target, sf::RenderStates states) const {
        target.draw(m_loadingText, states);
        target.draw(m_bestScoreText, states);
    }
}
