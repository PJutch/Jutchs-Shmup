/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef LANGUAGE_MANAGER_
#define LANGUAGE_MANAGER_

#include <filesystem>
#include <string>
#include <exception>

class LanguageManager {
public:
    LanguageManager() noexcept = default;

    // file format:
    // key1 value1, other parts of the value1
    // key2 value2
    //
    // key3 value3
    // (each key-value pair is on new line,
    //  empty lines are ignored
    //  key is separated from value with space,
    //  key can't contain spaces, 
    //  value can contain spaces but can't contain '\n',
    //  leading end ending spaces are stripped in both)
    void loadLanguage(const std::filesystem::path& path);

    const std::string& getMenuText() const noexcept {
        return m_menuText;
    }

    const std::string& getVolumeText() const noexcept {
        return m_volumeText;
    }

    const std::string& getResumeText() const noexcept {
        return m_resumeText;
    }

    const std::string& getExitText() const noexcept {
        return m_exitText;
    }

    const std::string& getEnglishText() const noexcept {
        return m_englishText;
    }

    const std::string& getRussianText() const noexcept {
        return m_russianText;
    }
private:
    std::string m_menuText;    // gui.menu.menu
    std::string m_volumeText;  // gui.menu.volume
    std::string m_resumeText;  // gui.menu.resume
    std::string m_exitText;    // gui.menu.exit
    std::string m_englishText; // gui.menu.english
    std::string m_russianText; // gui.menu.russian
};

class LanguageLoadError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

#endif