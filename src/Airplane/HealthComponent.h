/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software Foundation 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup.
If not, see <https://www.gnu.org/licenses/>. */

#ifndef AIRPLANE_HEALTH_COMPONENT_H_
#define AIRPLANE_HEALTH_COMPONENT_H_

#include <SFML/System.hpp>

namespace Airplane {
    class HealthComponent {
    public:
        HealthComponent() : m_health{0}, m_maxHealth{0}, m_damageCooldown{sf::Time::Zero} {}

        void update(sf::Time elapsedTime) noexcept {
            m_damageCooldown -= elapsedTime;
        }

        int getHealth() const noexcept {
            return m_health;
        }

        bool isDead() const noexcept {
            return m_health <= 0;
        }

        // return true on success
        bool tryHeal() noexcept { 
            if (m_health < m_maxHealth) {
                ++ m_health;
                return true;
            } else {
                return false;
            }
        }

        // return true if killed
        bool damage() noexcept {
            if (m_damageCooldown <= sf::Time::Zero) {
                m_damageCooldown = sf::seconds(0.1f);

                -- m_health;
                if (isDead()) return true;
            }
            return false;
        }
    private:
        int m_health;
        int m_maxHealth;

        sf::Time m_damageCooldown;

        void setMaxHealth(int maxHealth) noexcept {
            m_health = maxHealth;
            m_maxHealth = maxHealth;
        }

        friend class Builder;
    };
}

#endif