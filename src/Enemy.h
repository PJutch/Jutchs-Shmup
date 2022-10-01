#ifndef ENEMY_H_
#define ENEMY_H_

#include "Airplane.h"
#include "Entity.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>

class Enemy : public Airplane {
public:
    Enemy(sf::Vector2f position,
          const sf::Texture& texture, const sf::Texture& bulletTexture, 
          std::vector<std::unique_ptr<Entity>>& entities,
          float gameHeight) noexcept;

    void update(sf::Time elapsedTime) noexcept override;

    bool shouldBeDeleted() const noexcept override {
        return !m_alive;
    }
private:
    bool m_alive;
};

#endif