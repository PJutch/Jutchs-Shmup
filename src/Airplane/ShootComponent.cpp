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

#include "../SoundEffect.h"
#include "../Bullet.h"

namespace Airplane {
    ShootComponent::ShootComponent(Airplane& owner, GameState& gameState) noexcept : 
            m_shootCooldown{sf::Time::Zero}, m_owner{owner}, m_gameState{gameState} {};

    void ShootComponent::shoot(sf::Vector2f position) noexcept {
        m_gameState.getEntities().addEntity<Bullet>(m_owner.isOnPlayerSide(), position);
    }

    sf::FloatRect ShootComponent::getAffectedArea() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        if (m_owner.isOnPlayerSide()) {
            return {position.x - size.x / 2.f, position.y - size.y / 2.f, INFINITY, size.y};
        } else {
            return {position.x + size.x / 2.f, position.y - size.y / 2.f, -INFINITY, size.y};
        }
    }   

    sf::FloatRect ShootComponent::getStartShotBounds() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        return {position.x - size.x / 2.f, position.y - size.y / 2.f, size.x, size.y};
    }   

    sf::Vector2f ShootComponent::getShotSpeed() const noexcept {
        auto speed = Bullet::getSpeed();
        return {(m_owner.isOnPlayerSide() ? 1 : -1) * speed.x, speed.y};
    }

    void BasicShootComponent::tryShoot() noexcept {
        if (m_shootCooldown <= sf::Time::Zero) {
            shoot(m_owner.getPosition());
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
            m_shootCooldown = sf::seconds(0.25f);
        }
    }

    void TripleShootComponent::tryShoot() noexcept {
        if (m_shootCooldown <= sf::Time::Zero) {
            auto position = m_owner.getPosition();
            float height = m_owner.getGlobalBounds().height;

            shoot(position);
            shoot({position.x, position.y + height / 2});
            shoot({position.x, position.y - height / 2});

            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());

            m_shootCooldown = sf::seconds(0.5f);
        }
    }

    sf::FloatRect TripleShootComponent::getAffectedArea() const noexcept {
        auto position = m_owner.getPosition();
        float ownerHeight = m_owner.getGlobalBounds().height;
        auto size = Bullet::getSize(m_gameState);
        
        float areaHeight = ownerHeight + size.y;

        if (m_owner.isOnPlayerSide()) {
            return {position.x - size.x / 2.f, position.y - areaHeight / 2.f, INFINITY, areaHeight};
        } else {
            return {position.x + size.x / 2.f, position.y - areaHeight / 2.f, -INFINITY, areaHeight};
        }
    }

    sf::FloatRect TripleShootComponent::getStartShotBounds() const noexcept {
        auto position = m_owner.getPosition();
        auto size = Bullet::getSize(m_gameState);
        float ownerHeight = m_owner.getGlobalBounds().height;   

        float areaHeight = ownerHeight + size.y;

        return {position.x - size.x / 2.f, position.y - areaHeight / 2.f, size.x, areaHeight};
    }  

    void VolleyShootComponent::tryShoot() noexcept {
        if (m_shootCooldown <= sf::Time::Zero) {
            auto position = m_owner.getPosition();
            float height = m_owner.getGlobalBounds().height;

            shoot(m_owner.getPosition());

            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());

            m_shots += 2;
            m_shootCooldown = sf::seconds(0.1f);
        }
    }

    void VolleyShootComponent::update(sf::Time elapsedTime) noexcept {
        ShootComponent::update(elapsedTime);
        if (m_shootCooldown <= sf::Time::Zero && m_shots > 0) {
            shoot(m_owner.getPosition());
            m_gameState.getSounds().addSound(m_gameState.getAssets().getRandomShotSound());
            m_shootCooldown = sf::seconds(-- m_shots == 0 ? 0.5f : 0.1f);
        }
    }

    VolleyShootComponent::VolleyShootComponent(Airplane& owner, GameState& gameState) noexcept : 
        ShootComponent{owner, gameState}, m_shots{0} {};
    }
