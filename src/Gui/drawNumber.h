/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef DRAW_NUMBER_H_
#define DRAW_NUMBER_H_

#include "../AssetManager.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cmath>

namespace Gui {
    // draw n without sign 
    // UB if n < 0
    // return element size
    // element's origin at its top left corner
    // alpha in [0.f, 1.f]
    sf::Vector2f drawUnsignedNumber(int n, sf::Vector2f position, float alpha,
        sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets);

    inline sf::Vector2f drawUnsignedNumber(int n, sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        return drawUnsignedNumber(n, position, 1.f, target, states, assets);
    }

    namespace detail {
        sf::Vector2f drawNumberWithSign(int n, sf::Vector2f position, float alpha,
            sf::RenderTarget& target, sf::RenderStates states, 
            const sf::Texture& signTexture, const AssetManager& assets);
    }

    // draw n with sign if n < 0
    // return element size
    // element's origin at its top left corner
    // alpha in [0.f, 1.f]
    inline sf::Vector2f drawNumber(int n, sf::Vector2f position, float alpha,
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        if (n < 0) {
            return detail::drawNumberWithSign(-n, position, alpha, target, states, 
                                              assets.getMinusTexture(), assets);
        } else {
            return drawUnsignedNumber(n, position, alpha, target, states, assets);
        }
    }

    inline sf::Vector2f drawNumber(int n, sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        return drawNumber(n, position, 1.f, target, states, assets);
    }

    // draw n with sign
    // return element size
    // element's origin at its top left corner
    // alpha in [0.f, 1.f]
    inline sf::Vector2f drawSignedNumber(int n, sf::Vector2f position, float alpha,
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        return detail::drawNumberWithSign(std::abs(n), position, alpha, target, states, 
            (n < 0) ? assets.getMinusTexture() : assets.getPlusTexture(), assets);
    }

    inline sf::Vector2f drawSignedNumber(int n, sf::Vector2f position, 
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        return drawSignedNumber(n, position, 1.f, target, states, assets);
    }
}

#endif // DRAW_NUMBER_H_