# Jutch's Shump
 
## About
This is just a shoot'em up game.

Currently work is in progress.

(This branch contains buggy code that I want to explore some times a bit more).

## License
Jutch's Shump is licensed under GNU General Public License 3

## Dependencies
All dependencies are provided with this project in extlibs directory (as source or binaries)
There are:
- [SFML](https://www.sfml-dev.org)

## Credits
- [Kenney](https://www.kenney.nl) for [Pixel Shmup](https://kenney.nl/assets/pixel-shmup),
                                      [Sci-Fi Sounds](https://kenney.nl/assets/sci-fi-sounds),
                                      [Digital Audio](https://kenney.nl/assets/digital-audio)
                                  and [Ship Mixer](https://kenney.itch.io/ship-mixer)
- [Sogomn](https://opengameart.org/users/sogomn) for [explosion animation](https://opengameart.org/content/explosion-3)

## Build
You can build this project with CMake under Windows with MSVC.

Under Linux or with MinGW, you need to download or build SFML and link against it.
(It should work, but hasn't been tested)
May be it will be fixed in the future.

After build, copy the contents of extlibs/SFML/bin to the directory with executable.
