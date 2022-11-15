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

#include "Airplane.h"
#include "Gui/Text.h"
#include "Gui/Button.h"
#include "Gui/HorizontalSlider.h"
#include "Gui/ComboBox.h"

#include <SFML/Graphics.hpp>
using sf::RenderTarget;
using sf::RenderStates;
using sf::Sprite;
using sf::View;
using sf::Texture;
using sf::Image;
using sf::Color;

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;
using sf::Vector2u;
using sf::Vector2f;
using sf::IntRect;

#include <format>
using std::format;

#include <random>
using std::random_device;

#include <algorithm>
using std::erase_if;
using std::max;

#include <string>
using std::string;
using std::to_string;

using std::ssize;

#include <exception>

#include <memory>
using std::unique_ptr;
using std::make_unique;

#include <utility>
using std::move;

class GuiInvalidated : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

GameState::GameState(Vector2f screenSize) : 
        m_assetManager{}, m_tickClock{}, m_clock{}, 
        m_randomEngine{random_device{}()}, m_player{nullptr}, 
        m_screenSize{screenSize}, m_gameHeight{512}, m_spawnX{m_gameHeight * 4}, 
        m_score{0}, m_shouldResetAfter{Time::Zero}, m_sounds{}, m_volume{100.f},
        m_menuOpen{false},
        m_shouldEnd{false} {
    m_player = Airplane::Builder{*this}
        .position({0.f, 0.f}).maxHealth(3).deletable(false)
        .textureHeavy(true).textureFast(false).textureHasWeapon(false)
        .shootComponent<BasicShootComponent>().playerSide(true)
        .shootControlComponent<PlayerShootControlComponent>()
        .moveComponent<PlayerMoveComponent>().speed({250.f, 250.f})
        .addDeathEffect<LoseDeathEffect>().addDeathEffect<ExplosionDeathEffect>().build().release();
    m_entityManager.addEntity(m_player);

    m_languageManager.setLanguage(LanguageManager::Language::ENGLISH);
    initGui();
}

void GameState::initGui() noexcept {
    Vector2f menuSize{m_screenSize.y * 0.75f, m_screenSize.y * 0.75f};
    Vector2f menuPos = (m_screenSize - menuSize) / 2.f;
    m_menu.setRect({menuPos.x, menuPos.y, menuSize.x, menuSize.y});
    m_menu.setColor(Color{0, 0, 0, 128});
    m_menu.clearChildren();

    int characterSize = 100;
    const auto& font = m_assetManager.getFont();

    float buttonOutline = max(m_screenSize.y / 256.f, 0.5f);
    float buttonTextPadding = m_screenSize.y / 10.f;
    Vector2f buttonSize{buttonTextPadding, m_screenSize.y / 10.f}; // .x will be updated
    Color buttonColor{0, 0, 0, 128};
    Color buttonElementColor = Color::White;
    int buttonCharacterSize = 80;
    float buttonOffset = max(m_screenSize.y / 64.f, 1.f);

    float sliderHeight = max(m_screenSize.y / 64.f, 1.f);
    Vector2f sliderRunnerSize{max(m_screenSize.y / 64.f, 1.f), max(m_screenSize.y / 24.f, 1.f)};
    float sliderOffset = max(m_screenSize.y / 16.f, 1.f);
    float sliderLabelOffset = max(m_screenSize.y / 32.f, 1.f);
    int sliderCharacterSize = 50;

    Color comboColor{0, 0, 0, 128};
    Color comboElementColor = Color::White;
    Color comboSelectionColor{64, 96, 255};
    float comboOutline = -max(m_screenSize.y / 256.f, 0.5f);
    float comboTextPadding = m_screenSize.y / 10.f;
    Vector2f comboSize{comboTextPadding, m_screenSize.y / 10.f}; // .x will be updated
    float comboOffset = max(m_screenSize.y / 16.f, 1.f);
    float comboLabelOffset = max(m_screenSize.y / 32.f, 1.f);
    int comboCharacterSize = 50;

    auto menuText = make_unique<Gui::Text>(getLanguageManager().getMenuText(), 
                                            font, characterSize, Color::White);
    menuText->setOrigin({menuText->getSize().x / 2.f, 0.f});
    menuText->setPosition({menuSize.x / 2.f, 0.f}); // .x at center, .y at top
    
    auto volumeText = make_unique<Gui::Text>(getLanguageManager().getVolumeText(), 
                                                font, sliderCharacterSize, Color::White);

    // .x at center, .y under menuText
    volumeText->setOrigin({volumeText->getSize().x / 2.f, 0.f});
    volumeText->setPosition({menuSize.x / 2.f, menuText->getSize().y + sliderOffset});

    auto volumeSlider = make_unique<Gui::HorizontalSlider>([this]() -> float {
        return m_volume / 100.f;
    }, [this](float volume){
        m_volume = volume * 100.f;
    }, Color::White, Color::Black);
    volumeSlider->setSize({2.f / 3.f * menuSize.x, sliderHeight});

    // .x at center, .y under volumeText
    volumeSlider->setOrigin({volumeSlider->getSize().x / 2, 0.f});
    volumeSlider->setPosition({menuSize.x / 2.f, 
        menuText->getSize().y + sliderOffset + volumeText->getSize().y + sliderLabelOffset});
    
    volumeSlider->setRunnerSize(sliderRunnerSize);

    // move center to center of the line
    volumeSlider->setRunnerOrigin({sliderRunnerSize.x / 2.f,
                                   sliderRunnerSize.y / 2.f - sliderHeight / 2.f});

    auto englishText = make_unique<Gui::Text>(
        getLanguageManager().getLanguageName(LanguageManager::Language::ENGLISH), 
        font, comboCharacterSize, comboElementColor);
    englishText->setOrigin({englishText->getSize().x / 2.f, englishText->getSize().y * (2.f / 3.f)});
    englishText->setPosition({0.f, comboSize.y / 2.f}); // place in the center of the button
    comboSize.x = max(englishText->getSize().x + comboTextPadding, comboSize.x);

    auto russianText = make_unique<Gui::Text>(
        getLanguageManager().getLanguageName(LanguageManager::Language::RUSSIAN), 
        font, comboCharacterSize, comboElementColor);
    russianText->setOrigin({russianText->getSize().x / 2.f, russianText->getSize().y * (2.f / 3.f)});
    russianText->setPosition({0.f, comboSize.y / 2.f}); // place in the center of the button
    comboSize.x = max(russianText->getSize().x + comboTextPadding, comboSize.x);

    auto languageCombo = make_unique<Gui::ComboBox>([this]() -> int {
        return static_cast<int>(m_languageManager.getLanguage());
    }, [this](int current){
        m_languageManager.setLanguage(static_cast<LanguageManager::Language>(current));
        initGui();
        throw GuiInvalidated{"Gui reloaded"};
    }, comboColor, comboElementColor, comboOutline, comboSelectionColor);
    languageCombo->setSize(comboSize);
    languageCombo->addChild(std::move(englishText));
    languageCombo->addChild(std::move(russianText));

    // .x at center, .y under volumeSlider
    languageCombo->setOrigin({languageCombo->getSize().x / 2, 0.f});
    languageCombo->setPosition({menuSize.x / 2.f, 
        menuText->getSize().y + sliderOffset + volumeText->getSize().y 
        + sliderLabelOffset + sliderHeight + comboOffset});

    auto resumeText = make_unique<Gui::Text>(getLanguageManager().getResumeText(), 
                                                font, buttonCharacterSize, buttonElementColor);
    resumeText->setOrigin({resumeText->getSize().x / 2.f, resumeText->getSize().y});
    resumeText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
    buttonSize.x = max(resumeText->getSize().x + buttonTextPadding, buttonSize.x);

    auto exitText = make_unique<Gui::Text>(getLanguageManager().getExitText(), 
                                            font, buttonCharacterSize, buttonElementColor);
    exitText->setOrigin({exitText->getSize().x / 2.f, exitText->getSize().y});
    exitText->setPosition({0.f, -buttonSize.y / 2.f}); // place in the center of the button
    buttonSize.x = max(exitText->getSize().x + buttonTextPadding, buttonSize.x);

    auto resumeButton = make_unique<Gui::Button>([this]{
        m_menuOpen = false;
    }, buttonColor, buttonElementColor, buttonOutline, buttonOutline);
    resumeButton->setSize(buttonSize);

    // .x at center, .y above exitButton
    resumeButton->setOrigin({buttonSize.x / 2.f, buttonSize.y});
    resumeButton->setPosition({menuSize.x / 2.f, 
        menuSize.y- 2 * buttonOffset - 3 * buttonOutline - buttonSize.y}); 
    resumeButton->setChild(std::move(resumeText));

    auto exitButton = make_unique<Gui::Button>([this]{
        m_shouldEnd = true;
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

void GameState::handleEvent(sf::Event event) noexcept {
    if (m_menuOpen) {
        try {
            m_menu.handleEvent(event);
        } catch (const GuiInvalidated& e) {}
    } else {
        m_entityManager.handleEvent(event);
    }        

    if (event.type == sf::Event::Closed) m_shouldEnd = true;

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        m_menuOpen = !m_menuOpen;
}

void GameState::update() noexcept {
    Time elapsedTime = m_tickClock.restart();

    if (!m_menuOpen) m_entityManager.update(elapsedTime);

    erase_if(m_sounds, [this](const unique_ptr<SoundEffect>& sound) -> bool {
        return sound->hasStopped();
    });

    if (m_shouldResetAfter > Time::Zero) {
        m_shouldResetAfter -= elapsedTime;
        if (m_shouldResetAfter <= Time::Zero) reset();
    }

    while (m_player->getPosition().x + 4 * m_gameHeight > m_spawnX) {
        Vector2u enemySize = getAssets().getAirplaneTextureSize();
        for (float y = (enemySize.y - m_gameHeight) / 2; 
                y < (m_gameHeight- enemySize.y) / 2; y += enemySize.y) {
            trySpawnEnemy(Vector2f{m_spawnX, y});
        }
        m_spawnX += enemySize.x;
        addScore(1);
    }

    while (!m_scoreChanges.empty() 
            && m_scoreChanges.front().time + seconds(0.5f) < m_clock.getElapsedTime()) {
        m_score += m_scoreChanges.front().value;
        m_scoreChanges.pop_front();
    }
}

void GameState::reset() noexcept {  
    m_player->setPosition({0.f, 0.f});
    m_player->setHealth(3);

    m_entityManager.reset();

    m_spawnX = m_gameHeight * 4;

    m_score = 0;
    m_scoreChanges.clear();

    m_clock.restart();
}

bool GameState::inActiveArea(float x) const noexcept {
    return x + 2 * getGameHeight() >= getPlayer().getPosition().x 
        && x - 5 * getGameHeight() <= getPlayer().getPosition().x;
}

void GameState::drawNumber(int n, Vector2f position, 
                           RenderTarget& target, RenderStates states) const noexcept {
    auto score = to_string(n);
    auto digitSize = getAssets().getDigitTextures()[0].getSize();
    for (int i = 0; i < ssize(score); ++ i) {
        Sprite digitSprite{getAssets().getDigitTextures()[score[i] - '0']};
        digitSprite.setPosition(i * digitSize.x + position.x, position.y);
        target.draw(digitSprite, states);
    }
}

void GameState::draw(RenderTarget& target, RenderStates states) const noexcept {
    auto prevView = target.getView();
    target.setView(getView());

    target.draw(m_entityManager, states);

    target.setView(prevView);

    auto healthSize = 2u * getAssets().getHealthTexture().getSize();
    Sprite healthSprite{getAssets().getHealthTexture()};
    healthSprite.scale(2, 2);
    for (int i = 0; i < getPlayer().getHealth(); ++ i) {
        healthSprite.setPosition(i * healthSize.x, 0.f);
        target.draw(healthSprite, states);
    }

    drawNumber(m_score, Vector2f(0.f, healthSize.y), target, states);
    auto digitSize = getAssets().getDigitTextures()[0].getSize();
    for (int i = 0; i < ssize(m_scoreChanges); ++ i) {
        Sprite signSprite;
        if (m_scoreChanges[i].value >= 0) {
            signSprite.setTexture(getAssets().getPlusTexture());
        } else {
            signSprite.setTexture(getAssets().getMinusTexture());
        }
        signSprite.setPosition(0.f, healthSize.y + digitSize.y * (i + 1));
        target.draw(signSprite, states);

        drawNumber(m_scoreChanges[i].value, 
                   Vector2f(digitSize.x, healthSize.y + digitSize.y * (i + 1)), target, states);
    }

    if (m_menuOpen) target.draw(m_menu, states);;
}

View GameState::getView() const noexcept {
    return View{{getPlayer().getPosition().x - getGameHeight() / 2.f, 
                 -getGameHeight() / 2.f, 
                 getGameHeight() * getScreenSize().x / getScreenSize().y, 
                 getGameHeight()}};
}

void GameState::trySpawnEnemy(sf::Vector2f position) noexcept {
    std::uniform_real_distribution canonicalDistribution{0.0, 1.0};
    if (genRandom(canonicalDistribution) < 0.01) {
        Airplane::Builder builder{*this};

        builder.position(position).maxHealth(1).deletable(true).playerSide(false);

        int score = 10;

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.maxHealth(3).textureHeavy(true);
            score *= 2;
        } else {
            builder.maxHealth(1).textureHeavy(false);
        }

        double shootSeed = genRandom(canonicalDistribution);
        if (shootSeed < 0.1) {
            builder.shootComponent<TripleShootComponent>()
                   .textureHasWeapon(true);
            score *= 2;
        } else if (shootSeed < 0.2) {
            builder.shootComponent<VolleyShootComponent>()
                   .textureHasWeapon(false);
            score *= 2;
        } else {
            builder.shootComponent<BasicShootComponent >()
                   .textureHasWeapon(false);
        }

        double shootControlSeed = genRandom(canonicalDistribution);
        unique_ptr<ShootControlComponent> shootControl{nullptr};
        if (shootControlSeed < 0.1) {
            shootControl = builder.createShootControlComponent<TargetPlayerShootControlComponent>();
        } else if (shootControlSeed < 0.2) {
            shootControl = builder.createShootControlComponent<NeverShootControlComponent>();
            builder.textureHasWeapon(false);
            score /= 2;
        } else {
            shootControl = builder.createShootControlComponent<AlwaysShootControlComponent>();
        }

        builder.shootControlComponent<AndShootControlComponent>(move(shootControl), 
            builder.createShootControlComponent<CanHitPlayerShootControlComponent>());

        if (genRandom(canonicalDistribution) < 0.1) {
            builder.speed({500.f, 250.f}).textureFast(true);
            score *= 2;
        } else {
            builder.speed({250.f, 250.f}).textureFast(false);
        }

        double moveSeed = genRandom(canonicalDistribution);
        if (moveSeed < 0.1) {
            builder.moveComponent<PeriodicalMoveComponent>();
        } else if (moveSeed < 0.2) {
            builder.moveComponent<FollowPlayerMoveComponent>();
        } else {
            builder.moveComponent<BasicMoveComponent>();
        }

        builder.addDeathEffect<ScoreDeathEffect>(score)
               .addDeathEffect<LootDeathEffect>()
               .addDeathEffect<ExplosionDeathEffect>();

        m_entityManager.addEntity(builder.build());
    }
}
