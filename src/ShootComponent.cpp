/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "ShootComponent.h"

#include "Airplane.h"
#include "Player.h"

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;

#include <SFML/Graphics.hpp>
using sf::Texture;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

BasicShootComponent::BasicShootComponent(Airplane& owner, float gameHeight, 
                                         vector<unique_ptr<Entity>>& entities, 
                                         const Texture& bulletTexture, Player& player) noexcept : 
        m_shootCooldown{Time::Zero}, m_owner{owner}, m_gameHeight{gameHeight}, m_entities{entities}, 
        m_bulletTexture{bulletTexture}, m_player{player} {};

void BasicShootComponent::tryShoot(bool right) noexcept {
    if (m_shootCooldown <= Time::Zero) {
        m_entities.emplace_back(new Bullet{&m_owner, right, m_owner.getPosition(), 
                                           m_bulletTexture, m_player, m_gameHeight});
        m_shootCooldown = seconds(0.25f);
    }
}
