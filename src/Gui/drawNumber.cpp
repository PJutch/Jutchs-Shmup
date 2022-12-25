/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "drawNumber.h"

namespace Gui {
    sf::Vector2f drawUnsignedNumber(int n, sf::Vector2f position, float alpha,
            sf::RenderTarget& target, sf::RenderStates states, const AssetManager& assets) {
        auto string = std::to_string(n);
        const auto& digitTextures = assets.getDigitTextures();
        auto digitSize = digitTextures[0].getSize();

        for (int i = 0; i < std::ssize(string); ++ i) {
            sf::Sprite digitSprite{digitTextures[string[i] - '0']};
            digitSprite.setColor(sf::Color(255, 255, 255, alpha * 255));
            digitSprite.setPosition(i * digitSize.x + position.x, position.y);
            target.draw(digitSprite, states);
        }

        return sf::Vector2f(std::ssize(string) * digitSize.x, digitSize.y);
    }

    namespace detail {
        // draw n (with passed sign) 
        // UB if n < 0
        // return element size
        // element's origin at its top left corner
        // alpha in [0.f, 1.f]
        sf::Vector2f drawNumberWithSign(int n, sf::Vector2f position, float alpha,
                sf::RenderTarget& target, sf::RenderStates states, 
                const sf::Texture& signTexture, const AssetManager& assets) {
            auto signSize = signTexture.getSize();

            sf::Sprite signSprite{signTexture};
            signSprite.setPosition(position);
            target.draw(signSprite, states);

            sf::Vector2f numberPosition{position.x + signSize.x, position.y};
            return sf::Vector2f(signSize.x + drawUnsignedNumber(n, numberPosition, alpha, 
                target, states, assets).x, signSize.y);
        }
    }
}
