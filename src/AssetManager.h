/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef ASSET_MANAGER_
#define ASSET_MANAGER_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <random>
#include <vector>
#include <array>
#include <concepts>

class AssetManager {
public:
    AssetManager();

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

    const std::vector<sf::Texture>& getExplosionAnimation() const noexcept {
        return m_explosionAnimation;
    }

    template <std::uniform_random_bit_generator Engine>
    const sf::SoundBuffer& getRandomExplosionSound(Engine& engine) const noexcept {
        return m_explosionSounds[std::uniform_int_distribution<int>(
                                    0, std::ssize(m_explosionSounds) - 1)(engine)];
    }

    template <std::uniform_random_bit_generator Engine>
    const sf::SoundBuffer& getRandomShotSound(Engine& engine) const noexcept {
        return m_shotSounds[std::uniform_int_distribution<int>(
                            0, std::ssize(m_shotSounds) - 1)(engine)];
    }

    template <std::uniform_random_bit_generator Engine>
    const sf::SoundBuffer& getRandomPowerUpSound(Engine& engine) const noexcept {
        return m_powerUpSounds[std::uniform_int_distribution<int>(
                                0, std::ssize(m_powerUpSounds) - 1)(engine)];
    }
private:
    sf::Texture m_playerTexture;
    sf::Texture m_healthTexture;
    sf::Texture m_bulletTexture;
    sf::Texture m_enemyTexture;
    std::array<sf::Texture, 10> m_digitTextures;
    sf::Texture m_healthPickupTexture;
    std::vector<sf::Texture> m_explosionAnimation;

    std::array<sf::SoundBuffer, 5> m_explosionSounds;
    std::array<sf::SoundBuffer, 5> m_shotSounds;
    std::array<sf::SoundBuffer, 12> m_powerUpSounds;
};

class AssetLoadError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class TextureLoadError : public AssetLoadError {
    using AssetLoadError::AssetLoadError;
};

class SoundLoadError : public AssetLoadError {
    using AssetLoadError::AssetLoadError;
};

#endif