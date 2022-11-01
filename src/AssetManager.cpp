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
using sf::Image;

#include <SFML/System.hpp>
using sf::IntRect;

#include <format>
using std::format;

using std::ssize;

AssetManager::AssetManager() {
    if (!m_healthTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0026.png"))
        throw TextureLoadError{"Can't load health texture"};
    
    if (!m_bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png"))
        throw TextureLoadError{"Can't load bullet texture"};

    if (!m_healthPickupTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0024.png")) 
        throw TextureLoadError{"Can't load health pickup texture"};
    
    Image explosionAnimationMap;
    if (!explosionAnimationMap.loadFromFile("resources/explosion.png")) 
        throw TextureLoadError{"Can't load explosion animation"};
    for (unsigned int x = 0; x < explosionAnimationMap.getSize().x; 
            x += explosionAnimationMap.getSize().y) {
        m_explosionAnimation.emplace_back();
        m_explosionAnimation.back().loadFromImage(explosionAnimationMap, 
            IntRect(x, 0, explosionAnimationMap.getSize().y, explosionAnimationMap.getSize().y));
    }

    for (int playerSide = 0; playerSide < 2; ++ playerSide) 
    for (int heavy = 0;      heavy < 2;      ++ heavy) 
    for (int fast = 0;       fast < 2;       ++ fast) 
    for (int weapon = 0;     weapon < 2;     ++ weapon) 
        if (!m_airplaneTextures[playerSide][heavy][fast][weapon].loadFromFile(
                format("resources/Airplanes/airplane_{}{}{}{}.png", 
                    playerSide ? "player"  : "enemy", 
                    heavy  ? "_heavy"  : "", 
                    fast   ? "_fast"   : "", 
                    weapon ? "_weapon" : "")))
            throw TextureLoadError{format("Can't load {}{}{} airplane texture {} weapon", 
                                          playerSide ? "player" : "enemy",
                                          heavy ?      " heavy" : "",
                                          fast  ?      " fast"  : "", 
                                          weapon ?     "with"   : "without")};
    
    if (!m_plusTexture.loadFromFile("resources/plus.png"))
        throw TextureLoadError{"Can't load plus {} texture"};
    
    if (!m_minusTexture.loadFromFile("resources/minus.png"))
        throw TextureLoadError{"Can't load minus {} texture"};

    for (int i = 0; i < ssize(m_digitTextures); ++ i) 
        if (!m_digitTextures[i].loadFromFile(
                    format("resources/kenney_pixelshmup/Digits/digit_{}.png", i))) 
            throw TextureLoadError{format("Can't load digit {} texture", i)};

    for (int i = 0; i < ssize(m_explosionSounds); ++ i) 
        if (!m_explosionSounds[i].loadFromFile(
                format("resources/sci-fi-sounds/Explosions/explosionCrunch_{}.ogg", i))) 
            throw SoundLoadError{format("Can't load explosion sound {}", i)};

    for (int i = 0; i < ssize(m_shotSounds); ++ i) 
        if (!m_shotSounds[i].loadFromFile(
                format("resources/sci-fi-sounds/Shots/laserSmall_{}.ogg", i))) 
            throw SoundLoadError{format("Can't load shot sound {}", i)};

    for (int i = 0; i < ssize(m_powerUpSounds); ++ i) 
        if (!m_powerUpSounds[i].loadFromFile(
                format("resources/kenney_digitalaudio/PowerUp/powerUp{}.ogg", i + 1))) 
            throw SoundLoadError{format("Can't load power up sound {}", i)};
}

const sf::Texture& AssetManager::getAirplaneTexture(
        bool playerSide, 
        bool heavy, 
        bool fast, 
        bool hasWeapon) const noexcept {
    return m_airplaneTextures[static_cast<int>(playerSide)]
                             [static_cast<int>(heavy)]
                             [static_cast<int>(fast)]
                             [static_cast<int>(hasWeapon)];
}
