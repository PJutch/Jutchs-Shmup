/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef SOUND_EFFECT_H_
#define SOUND_EFFECT_H_

#include <SFML/Audio.hpp>

class SoundEffect {
public:
    SoundEffect(const sf::SoundBuffer& sound) noexcept : m_sound{sound} {
        m_sound.play();
    }

    bool hasStopped() const noexcept {
        return m_sound.getStatus() == sf::Sound::Stopped;
    }
private:
    sf::Sound m_sound;
};

#endif
