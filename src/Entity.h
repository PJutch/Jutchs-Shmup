#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Airplane;
class Bullet;

class Entity : public sf::Drawable {
public:
    virtual ~Entity() = default;

    virtual void update(sf::Time elapsedTime) noexcept = 0;

    virtual sf::FloatRect getGlobalBounds() const noexcept = 0;

    virtual void startCollide(Entity& other) noexcept = 0;
    virtual void acceptCollide(Airplane& other) noexcept = 0;
    virtual void acceptCollide(Bullet& other) noexcept = 0;

    virtual bool shouldBeDeleted() const noexcept = 0;
};

#endif