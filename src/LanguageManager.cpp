/* This file is part of Jutchs Shmup.

Jutchs Shmup is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

Jutchs Shmup is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Jutchs Shmup. 
If not, see <https://www.gnu.org/licenses/>. */

#include "LanguageManager.h"

#include <fstream>
#include <algorithm>
#include <locale>
#include <string_view>
#include <array>

LanguageManager::LanguageManager() noexcept : m_language{LanguageManager::Language::TOTAL} {}

void LanguageManager::loadLanguage(const std::filesystem::path& path) {
    std::ifstream file{path};
    std::string line;

    std::locale locale;
    file.imbue(locale);
    auto isspace = [&locale](char c) -> bool {
        return std::isspace(c, locale);
    };

    // each key-value pair is on new line
    while (std::getline(file, line)) {
        // strip spaces from left and right
        auto strippedBegin = std::ranges::find_if_not(line, isspace);
        if (strippedBegin == line.end()) continue; // don't try to parse empty line

        auto strippedEnd = std::ranges::find_if_not(line.rbegin(), line.rend(), isspace).base();

        // key and value are separated with spaces
        auto keyEnd = std::ranges::find_if(line, isspace);
        std::string_view key{strippedBegin, keyEnd};

        // leading spaces are stripped
        auto valueBegin = std::ranges::find_if_not(keyEnd, line.end(), isspace);
        std::string_view value{valueBegin, strippedEnd};

               if (key == "gui.menu.menu") {
            m_menuText = value;
        } else if (key == "gui.menu.volume") {
            m_volumeText = value;
        } else if (key == "gui.menu.resume") {
            m_resumeText = value;
        } else if (key == "gui.menu.exit") {
            m_exitText = value;
        } else if (key == "gui.loading.loading") {
            m_loadingText = value;
        } else if (key == "gui.loading.best_score") {
            m_bestScoreText = value;
        } else {
            bool found = true;
            for (int i = 0; i < static_cast<int>(Language::TOTAL); ++ i) {
                if (key == s_languageKeys[i]) {
                    m_languageNames[i] = value;
                }
            }
            if (!found) 
                throw LanguageLoadError
                    {std::format("Unknown key \"{}\" with value \"{}\"", key, value)};
        }
    }
}

const std::array<std::filesystem::path, static_cast<int>(LanguageManager::Language::TOTAL)> 
    LanguageManager::s_languagePaths{"resources/lang/en.lang", "resources/lang/ru.lang"};

const std::array<std::string, static_cast<int>(LanguageManager::Language::TOTAL)> 
    LanguageManager::s_languageKeys{"gui.menu.language.english", "gui.menu.language.russian"};
