/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SOUND_MANAGER_
#define SOUND_MANAGER_

#include "SoundEffect.h"

#include <SFML/Audio.hpp>

#include <vector>
#include <memory>

class SoundManager {
public:
    SoundManager() : m_volume{100.f} {}

    void update() {
        std::erase_if(m_sounds, [this](const std::unique_ptr<SoundEffect>& sound) -> bool {
            return sound->hasStopped();
        });
    }

    void addSound(std::unique_ptr<SoundEffect> sound) noexcept {
        sound->setVolume(m_volume * sound->getVolume());
        m_sounds.push_back(std::move(sound));
    }

    void addSound(const sf::SoundBuffer& sound) noexcept {
        addSound(std::make_unique<SoundEffect>(sound));
    }

    float getVolume() const noexcept {
        return m_volume;
    }

    void setVolume(float volume) noexcept {
        m_volume = volume;
    }
private:
    std::vector<std::unique_ptr<SoundEffect>> m_sounds;
    float m_volume;
};

#endif