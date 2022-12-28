/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "EntityManager.h"

#include "GameState.h"

#include "Airplane/Airplane.h"
#include "Airplane/Builder.h"
#include "Airplane/Components.h"

#include "geometry.h"

#include <array>

const int PLAYER_MAX_HEALTH = 3;
const sf::Vector2f PLAYER_START_POSITION{0.f, 0.f};

EntityManager::EntityManager(GameState& gameState) noexcept : 
    m_playerPosition{PLAYER_START_POSITION}, 
    m_playerGlobalBounds{PLAYER_START_POSITION.x, PLAYER_START_POSITION.y, 0.f, 0.f},
    m_gameState{gameState} {}

void EntityManager::init() {
    spawnPlayer();
    m_spawnX = 4 * m_gameState.getGameHeight();
}

namespace {
    std::array<Airplane::ShootComponent::PatternElement, 1> basicPattern {
        Airplane::ShootComponent::PatternElement{{0.f, 0.f}, sf::seconds(0.25f)}
    };

    std::array<Airplane::ShootComponent::PatternElement, 3> triplePattern {
        Airplane::ShootComponent::PatternElement{{0.f,   0.f}, sf::seconds(0.0f)},
        Airplane::ShootComponent::PatternElement{{0.f,  32.f}, sf::seconds(0.0f)},
        Airplane::ShootComponent::PatternElement{{0.f, -32.f}, sf::seconds(0.5f)}
    };

    std::array<Airplane::ShootComponent::PatternElement, 3> volleyPattern {
        Airplane::ShootComponent::PatternElement{{0.f, 0.f}, sf::seconds(0.1f)},
        Airplane::ShootComponent::PatternElement{{0.f, 0.f}, sf::seconds(0.1f)},
        Airplane::ShootComponent::PatternElement{{0.f, 0.f}, sf::seconds(0.5f)}
    };
}

void EntityManager::spawnPlayer() {
    using enum Airplane::Flags;

    m_player = Airplane::Builder{m_gameState}
        .position(PLAYER_START_POSITION).maxHealth(PLAYER_MAX_HEALTH)
        .flags(PLAYER_SIDE | HEAVY | SLOW | NO_WEAPON | USE_PICKUPS | NO_BOMB)
        .shootPattern(basicPattern)
        .shootControlComponent<Airplane::PlayerShootControlComponent>()
        .moveComponent<Airplane::PlayerMoveComponent>().speed(250.f, 250.f)
        .bombComponent<Airplane::PlayerBombComponent>()
        .addDeathEffect<Airplane::LoseDeathEffect>()
        .addDeathEffect<Airplane::ExplosionDeathEffect>()
        .build().release();
    addEntity(m_player);

    m_playerPosition = PLAYER_START_POSITION;
    m_playerGlobalBounds = m_player->getGlobalBounds();
}

sf::Vector2f EntityManager::getPlayerPosition() const noexcept {
    return m_playerPosition;
}

sf::FloatRect EntityManager::getPlayerGlobalBounds() const noexcept {
    return m_playerGlobalBounds;
}

int EntityManager::getPlayerHealth() const noexcept {
    return m_player ? m_player->getHealth() : 0;
}

void EntityManager::handleEvent(sf::Event event) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i)  {
        if (!m_entities[i]->shouldBeDeleted()) 
            m_entities[i]->handleEvent(event);
    }
}

void EntityManager::update(sf::Time elapsedTime) noexcept {
    for (int i = 0; i < ssize(m_entities); ++ i) 
        if (!m_entities[i]->shouldBeDeleted()) 
            m_entities[i]->update(elapsedTime);

    if (m_player) {
        m_playerPosition = m_player->getPosition();
        m_playerGlobalBounds = m_player->getGlobalBounds();
    }
    
    for (int i = 0; i < ssize(m_entities); ++ i) 
        for (int j = i + 1; j < ssize(m_entities); ++ j) 
            if (!m_entities[i]->shouldBeDeleted() 
             && !m_entities[j]->shouldBeDeleted() 
             && intersects(m_entities[i]->getGlobalBounds(), 
                           m_entities[j]->getGlobalBounds())) {
                m_entities[i]->startCollide(*m_entities[j]);
                m_entities[j]->startCollide(*m_entities[i]);
    }

    if (m_player && m_player->shouldBeDeleted()) {
        m_player = nullptr;

        auto [x, y] = getPlayerPosition();
        m_playerGlobalBounds = {x, y, 0.f, 0.f};
    }

    erase_if(m_entities, [this](const std::unique_ptr<Entity>& entity) -> bool {
        return entity->shouldBeDeleted();
    });

    checkEnemySpawn();
}

void EntityManager::draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
    for (const auto& entity : m_entities) {
        if (!entity->shouldBeDeleted()) 
            target.draw(*entity, states);
    }
}

void EntityManager::reset() noexcept {
    m_entities.clear();
    spawnPlayer();
    m_spawnX = 4 * m_gameState.getGameHeight();
}

void EntityManager::checkEnemySpawn() {
    while (getPlayerPosition().x + 4 * m_gameState.getGameHeight() > m_spawnX) {
        sf::Vector2u enemySize = m_gameState.getAssets().getAirplaneTextureSize();
        for (float y = (enemySize.y - m_gameState.getGameHeight()) / 2; 
                y < (m_gameState.getGameHeight() - enemySize.y) / 2; y += enemySize.y) {
            if (std::uniform_real_distribution{0.0, 1.0}(m_gameState.getRandomEngine()) < 0.01)
                spawnEnemy(sf::Vector2f{m_spawnX, y});
        }
        m_spawnX += enemySize.x;
    }
}

void EntityManager::spawnEnemy(sf::Vector2f position) {
    using enum Airplane::Flags;

    std::uniform_real_distribution canonicalDistribution{0.0, 1.0};

    Airplane::Builder builder{m_gameState};
    builder.position(position).maxHealth(1).flags(ENEMY_SIDE | NO_PICKUPS);

    int score = 10;

    if (canonicalDistribution(m_gameState.getRandomEngine()) < 0.1) {
        builder.maxHealth(3);
        builder.flags() |= HEAVY;
        score *= 2;
    } else {
        builder.maxHealth(1);
        builder.flags() |= LIGHT;
    }

    double shootSeed = canonicalDistribution(m_gameState.getRandomEngine());
    bool advancedWeapon = false;
    if (shootSeed < 0.1) {
        builder.shootPattern(triplePattern);
        builder.flags() |= HAS_WEAPON;
        advancedWeapon = true;
    } else if (shootSeed < 0.2) {
        builder.shootPattern(volleyPattern);
        builder.flags() |= NO_WEAPON;
        advancedWeapon = true;
    } else {
        builder.shootPattern(basicPattern);
        builder.flags() |= NO_WEAPON;
    }

    double shootControlSeed = canonicalDistribution(m_gameState.getRandomEngine());
    if (shootControlSeed < 0.1) {
        auto targetPlayer = builder.createComponent<Airplane::TargetPlayerShootControlComponent>();
        auto canHitPlayer = builder.createComponent<Airplane::CanHitPlayerShootControlComponent>();
        builder.shootControlComponent(targetPlayer && canHitPlayer);
    } else if (shootControlSeed < 0.2) {
        builder.shootControlComponent<Airplane::NeverShootControlComponent>();

        builder.flags() &= ~HAS_WEAPON;
        builder.flags() |= NO_WEAPON;
        advancedWeapon = false;
        score /= 2;
    } else {
        builder.shootControlComponent<Airplane::CanHitPlayerShootControlComponent>();
    }

    if (advancedWeapon) score *= 2;

    if (canonicalDistribution(m_gameState.getRandomEngine()) < 0.1) {
        builder.speed(500.f, 250.f);
        builder.flags() |= FAST;
        score *= 2;
    } else {
        builder.speed(250.f, 250.f);
        builder.flags() |= SLOW;
    }

    bool hasBomb = false;
    if (canonicalDistribution(m_gameState.getRandomEngine()) < 0.1) {
        builder.bomb();
        hasBomb = true;
        score *= 2;
    }
    builder.bombComponent<Airplane::EnemyBombComponent>();

    if (hasBomb && canonicalDistribution(m_gameState.getRandomEngine()) < 0.9) {
        builder.moveComponent<Airplane::LineWithTargetMoveComponent>(
            [&land = m_gameState.getLand(), 
             target = m_gameState.getLand().getTargetFor(position)] 
            (const Airplane::Airplane& owner) mutable -> sf::Vector2f {
                if (target.x > owner.getPosition().x && owner.hasBomb())
                    target = land.getTargetFor(owner.getPosition());
                
                return target;
            });
    } else {
        double moveSeed = canonicalDistribution(m_gameState.getRandomEngine());
        if (moveSeed < 0.1) {
            builder.moveComponent<Airplane::PeriodicalMoveComponent>();
        } else if (moveSeed < 0.2) {
            builder.moveComponent<Airplane::LineWithTargetMoveComponent>(
                [this](const Airplane::Airplane&) -> sf::Vector2f {
                    return getPlayerPosition();
                });
        } else {
            builder.moveComponent<Airplane::BasicMoveComponent>();
        }
    }

    builder.addDeathEffect<Airplane::ScoreDeathEffect>(score)
           .addDeathEffect<Airplane::LootDeathEffect>()
           .addDeathEffect<Airplane::ExplosionDeathEffect>();

    addEntity(builder.build());
}
