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

AssetManager::AssetManager() : 
        m_playerTexture{}, m_healthTexture{}, m_bulletTexture{}, m_enemyTexture{},
        m_digitTextures{}, m_healthPickupTexture{}, m_explosionAnimation{} {
    if (!m_playerTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0000.png")) 
        throw TextureLoadError{"Can't load player texture"};
    
    if (!m_healthTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0026.png"))
        throw TextureLoadError{"Can't load health texture"};
    
    if (!m_bulletTexture.loadFromFile("resources/kenney_pixelshmup/Tiles/tile_0000.png"))
        throw TextureLoadError{"Can't load bullet texture"};
    
    if (!m_enemyTexture.loadFromFile("resources/kenney_pixelshmup/Ships/ship_0001.png"))
        throw TextureLoadError{"Can't load enemy texture"};
    
    for (int i = 0; i < ssize(m_digitTextures); ++ i) 
        if (!m_digitTextures[i].loadFromFile(
                format("resources/kenney_pixelshmup/Digits/digit_{}.png", i))) 
            throw TextureLoadError{format("Can't load digit {} texture", i)};

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
}