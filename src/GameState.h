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

#include "Entity.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <random>
#include <vector>
#include <array>
#include <memory>
#include <exception>

class Airplane;

class GameState : public sf::Drawable {
public:
    GameState(sf::Vector2f screenSize);

    const sf::Texture& getPlayerTexture() const noexcept {
        return m_playerTexture;
    }

    const sf::Texture& getHealthTexture() const noexcept {
        return m_healthTexture;
    }

    const sf::Texture& getBulletTexture() const noexcept {
        return m_bulletTexture;
    }

    const sf::Texture& getEnemyTexture() const noexcept {
        return m_enemyTexture;
    }

    const std::array<sf::Texture, 10>& getDigitTextures() const noexcept {
        return m_digitTextures;
    }

    const sf::Texture& getHealthPickupTexture() const noexcept {
        return m_healthPickupTexture;
    }

    template<typename Distribution>
    auto genRandom(Distribution distibution) noexcept {
        return distibution(m_randomEngine);
    }

    float getGameHeight() const noexcept {
        return m_gameHeight;
    }

    const std::vector<std::unique_ptr<Entity>>& getEntities() const noexcept {
        return m_entities;
    }

    void addEntity(Entity* entity) noexcept {
        m_entities.emplace_back(entity);
    }

    void addEntity(std::unique_ptr<Entity>&& entity) noexcept {
        m_entities.push_back(std::move(entity));
    }

    const Airplane& getPlayer() const noexcept {
        return *m_player;
    }

    Airplane& getPlayer() noexcept {
        return *m_player;
    }

    void addScore(int score) noexcept {
        m_score += score;
    }

    sf::Vector2f getScreenSize() const noexcept {
        return m_screenSize;
    }

    bool inActiveArea(float x) const noexcept;

    void setShouldReset() noexcept {
        m_shouldReset = true;
    }

    void handleEvent(sf::Event event) noexcept {
        for (const auto& entity : getEntities()) {
            entity->handleEvent(event);
        }
    }

    void update(sf::Time elapsedTime) noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
private:
    sf::Texture m_playerTexture;
    sf::Texture m_healthTexture;
    sf::Texture m_bulletTexture;
    sf::Texture m_enemyTexture;
    std::array<sf::Texture, 10> m_digitTextures;
    sf::Texture m_healthPickupTexture;

    std::mt19937_64 m_randomEngine;

    std::vector<std::unique_ptr<Entity>> m_entities;
    Airplane* m_player;

    int m_score;

    bool m_shouldReset;

    sf::Vector2f m_screenSize;
    float m_gameHeight;
    float m_spawnX;

    sf::View getView() const noexcept;

    void reset() noexcept;

    void trySpawnEnemy(sf::Vector2f position) noexcept;
};

class TextureLoadError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

#endif