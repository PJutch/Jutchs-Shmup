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

#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Vector2f;

#include <SFML/System.hpp>
using sf::Time;
using sf::seconds;

#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

ShootComponent::ShootComponent(Airplane& owner, GameState& gameState) noexcept : 
        m_shootCooldown{Time::Zero}, m_owner{owner}, m_gameState{gameState} {};

void ShootComponent::shoot(bool right, Vector2f position) noexcept {
    m_gameState.addEntity(new Bullet{&m_owner, right, position, m_gameState});
}

void BasicShootComponent::tryShoot(bool right) noexcept {
    if (m_shootCooldown <= Time::Zero) {
        shoot(right, m_owner.getPosition());
        m_shootCooldown = seconds(0.25f);
    }
}

void TripleShootComponent::tryShoot(bool right) noexcept {
    if (m_shootCooldown <= Time::Zero) {
        auto position = m_owner.getPosition();
        float height = m_owner.getGlobalBounds().height;

        shoot(right, m_owner.getPosition());
        shoot(right, {position.x, position.y + height / 2});
        shoot(right, {position.x, position.y - height / 2});

        m_shootCooldown = seconds(0.5f);
    }
}

void VolleyShootComponent::tryShoot(bool right) noexcept {
    if (m_shootCooldown <= Time::Zero) {
        auto position = m_owner.getPosition();
        float height = m_owner.getGlobalBounds().height;

        shoot(right, m_owner.getPosition());

        m_shots += 2;
        m_right = right;
        m_shootCooldown = seconds(0.1f);
    }
}

void VolleyShootComponent::update(sf::Time elapsedTime) noexcept {
    ShootComponent::update(elapsedTime);
    if (m_shootCooldown <= sf::Time::Zero && m_shots > 0) {
        shoot(m_right, m_owner.getPosition());
        m_shootCooldown = sf::seconds(-- m_shots == 0 ? 0.5f : 0.1f);
    }
}

VolleyShootComponent::VolleyShootComponent(Airplane& owner, GameState& gameState) noexcept : 
    ShootComponent{owner, gameState}, m_shots{0}, m_right{false} {};
