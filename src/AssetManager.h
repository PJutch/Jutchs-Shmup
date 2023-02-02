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

#include "Airplane/Flags.h"
#include "Land.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <random>
#include <vector>
#include <array>
#include <concepts>

class AssetManager {
public:
    AssetManager(std::mt19937_64& randomEngine);

    const sf::Texture& getBulletTexture() const noexcept {
        return m_bulletTexture;
    }

    const sf::Texture& getBombTexture() const noexcept {
        return m_bombTexture;
    }

    const sf::Texture& getTurretTexture() const noexcept {
        return m_turretTexture;
    }

    const sf::Texture& getHealthPickupTexture() const noexcept {
        return m_healthPickupTexture;
    }

    const sf::Texture& getBombPickupTexture() const noexcept {
        return m_bombTexture;
    }

    const std::vector<sf::Texture>& getExplosionAnimation() const noexcept {
        return m_explosionAnimation;
    }

    const sf::Texture& getAirplaneTexture(Airplane::Flags flags) const noexcept {
        using Base = std::underlying_type_t<Airplane::Flags>;
        return m_airplaneTextures[static_cast<Base>(flags & Airplane::Flags::TEXTURE)];
    }
    
    sf::Vector2u getAirplaneTextureSize() const noexcept {
        return m_airplaneTextures[0].getSize();
    }

    const sf::Texture& getLandTexture(Land land) const noexcept {
        return m_landTextures[static_cast<std::underlying_type_t<Land>>(land)];
    }

    sf::Vector2u getLandTextureSize() const noexcept {
        return m_landTextures[0].getSize();
    }

    const sf::Texture& getHealthTexture() const noexcept {
        return m_healthTexture;
    }

    const sf::Texture& getPlusTexture() const noexcept {
        return m_plusTexture;
    }

    const sf::Texture& getMinusTexture() const noexcept {
        return m_minusTexture;
    }

    const std::array<sf::Texture, 10>& getDigitTextures() const noexcept {
        return m_digitTextures;
    }

    const sf::SoundBuffer& getRandomExplosionSound() const noexcept {
        auto ditribution = std::uniform_int_distribution<int64_t>(0, std::ssize(m_explosionSounds) - 1);
        return m_explosionSounds[ditribution(m_randomEngine)];
    }

    const sf::SoundBuffer& getRandomShotSound() const noexcept {
        auto ditribution = std::uniform_int_distribution<int64_t>(0, std::ssize(m_shotSounds) - 1);
        return m_shotSounds[ditribution(m_randomEngine)];
    }

    const sf::SoundBuffer& getRandomPowerUpSound() const noexcept {
        auto ditribution = std::uniform_int_distribution<int64_t>(0, std::ssize(m_powerUpSounds) - 1);
        return m_powerUpSounds[ditribution(m_randomEngine)];
    }

    const sf::Font& getFont() const noexcept {
        return m_font;
    }
private:
    sf::Texture m_bulletTexture;
    sf::Texture m_bombTexture;

    sf::Texture m_turretTexture;
    
    sf::Texture m_healthPickupTexture;
    std::vector<sf::Texture> m_explosionAnimation;

    std::array<sf::Texture, Airplane::TEXTURE_VARIANTS> m_airplaneTextures;

    std::array<sf::Texture, LAND_VARIANTS> m_landTextures;

    sf::Texture m_healthTexture;
    sf::Texture m_plusTexture;
    sf::Texture m_minusTexture;
    std::array<sf::Texture, 10> m_digitTextures;

    std::array<sf::SoundBuffer, 5> m_explosionSounds;
    std::array<sf::SoundBuffer, 5> m_shotSounds;
    std::array<sf::SoundBuffer, 12> m_powerUpSounds;

    sf::Font m_font;

    std::mt19937_64& m_randomEngine;
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

class FontLoadError : public AssetLoadError {
    using AssetLoadError::AssetLoadError;
};

#endif