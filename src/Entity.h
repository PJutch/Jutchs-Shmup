#ifndef ENTITY_H_
#define ENTITY_H_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Entity : public sf::Drawable {
public:
    virtual ~Entity() = default;

    virtual void update(sf::Time elapsedTime) noexcept = 0;

    virtual bool shouldBeDeleted() const noexcept = 0;
};

#endif