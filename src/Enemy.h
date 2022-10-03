#ifndef ENEMY_H_
#define ENEMY_H_

#include "Airplane.h"
#include "Entity.h"
#include "Player.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>

class Enemy : public Airplane {
public:
    Enemy(sf::Vector2f position, Player& player, 
          const sf::Texture& texture, const sf::Texture& bulletTexture, 
          std::vector<std::unique_ptr<Entity>>& entities,
          float gameHeight) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive;
    }

    void handleDamaged() noexcept override {
        m_alive = false;
        m_player.setScore(m_player.getScore() + 10);
    }
private:
    bool m_alive;
    Player& m_player;
};

#endif