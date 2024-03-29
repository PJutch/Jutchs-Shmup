/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "AssetManager.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <format>

AssetManager::AssetManager(std::mt19937_64& randomEngine) : m_randomEngine{randomEngine} {    
    if (!m_bulletTexture.loadFromFile("resources/textures/kenney_pixelshmup/Tiles/tile_0000.png"))
        throw TextureLoadError{"Can't load bullet texture"};

    if (!m_bombTexture.loadFromFile("resources/textures/bomb2.png"))
        throw TextureLoadError{"Can't load bomb texture"};

    if (!m_turretTexture.loadFromFile("resources/textures/kenney_pixelshmup/Tiles/tile_0018.png"))
        throw TextureLoadError{"Can't load turret texture"};

    if (!m_turretBaseTexture.loadFromFile("resources/textures/kenney_pixelshmup/Tiles/tile_0016.png"))
        throw TextureLoadError{"Can't load turret base texture"};

    if (!m_healthPickupTexture.loadFromFile("resources/textures/kenney_pixelshmup/Tiles/tile_0024.png")) 
        throw TextureLoadError{"Can't load health pickup texture"};
    
    sf::Image explosionAnimationMap;
    if (!explosionAnimationMap.loadFromFile("resources/textures/explosion.png")) 
        throw TextureLoadError{"Can't load explosion animation"};
    for (unsigned int x = 0; x < explosionAnimationMap.getSize().x; 
            x += explosionAnimationMap.getSize().y) {
        m_explosionAnimation.emplace_back();
        m_explosionAnimation.back().loadFromImage(explosionAnimationMap, 
            sf::IntRect(x, 0, explosionAnimationMap.getSize().y, explosionAnimationMap.getSize().y));
    }

    for (int i = 0; i < Airplane::TEXTURE_VARIANTS; ++ i) {
        auto flags = static_cast<Airplane::Flags>(i);
        if (!m_airplaneTextures[i]
            .loadFromFile(("resources/textures/Airplanes" / getTextureFileName(flags)).generic_string()))
            throw TextureLoadError{std::format("Can't load {} airplane texture", getTextureName(flags))};
    }

    forValidLand([&landTextures = m_landTextures](Land land) {
        if (!landTextures[static_cast<std::underlying_type_t<Land>>(land)]
            .loadFromFile(("resources/textures/Land/" / getTextureFileName(land)).generic_string()))
            throw TextureLoadError{std::format("Can't load {} tile texture", getName(land))};
    });

    if (!m_healthTexture.loadFromFile("resources/textures/kenney_pixelshmup/Tiles/tile_0026.png"))
        throw TextureLoadError{"Can't load health texture"};
    
    if (!m_plusTexture.loadFromFile("resources/textures/plus.png"))
        throw TextureLoadError{"Can't load plus texture"};
    
    if (!m_minusTexture.loadFromFile("resources/textures/minus.png"))
        throw TextureLoadError{"Can't load minus texture"};

    if (!m_slashTexture.loadFromFile("resources/textures/slash.png"))
        throw TextureLoadError{"Can't load slash texture"};

    for (int i = 0; i < std::ssize(m_digitTextures); ++ i) 
        if (!m_digitTextures[i].loadFromFile(
                std::format("resources/textures/Digits/digit_{}.png", i))) 
            throw TextureLoadError{std::format("Can't load digit {} texture", i)};
    
    for (int i = 0; i < std::size(m_explosionSounds); ++ i) 
        if (!m_explosionSounds[i].loadFromFile(
                std::format("resources/sounds/sci-fi-sounds/Explosions/explosionCrunch_{}.ogg", i))) 
            throw SoundLoadError{std::format("Can't load explosion sound {}", i)};

    for (int i = 0; i < std::ssize(m_shotSounds); ++ i) 
        if (!m_shotSounds[i].loadFromFile(
                std::format("resources/sounds/sci-fi-sounds/Shots/laserSmall_{}.ogg", i))) 
            throw SoundLoadError{std::format("Can't load shot sound {}", i)};

    for (int i = 0; i < std::ssize(m_powerUpSounds); ++ i) 
        if (!m_powerUpSounds[i].loadFromFile(
                std::format("resources/sounds/kenney_digitalaudio/PowerUp/powerUp{}.ogg", i + 1))) 
            throw SoundLoadError{std::format("Can't load power up sound {}", i)};
    
    if (!m_font.loadFromFile("resources/fonts/Roboto/Roboto-Medium.ttf")) 
        throw FontLoadError{std::format("Can't load font")};
}
