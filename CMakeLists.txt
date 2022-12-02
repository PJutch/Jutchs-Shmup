cmake_minimum_required(VERSION 3.10)

project(Jutchs_Shmup VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)
include_directories(extlibs/SFML/include)

add_executable(Jutchs_Shmup src/main.cpp 
                            src/Bullet.cpp
                            src/Airplane/ShootComponent.cpp 
                            src/GameState.cpp 
                            src/Airplane/MoveComponent.cpp 
                            src/Airplane/DeathEffect.cpp
                            src/Airplane/ShootControlComponent.cpp 
                            src/AnimatedParticle.cpp 
                            src/AssetManager.cpp 
                            src/EntityManager.cpp 
                            src/Gui/Panel.cpp 
                            src/util.cpp
                            src/Gui/Button.cpp 
                            src/Gui/HorizontalSlider.cpp 
                            src/LanguageManager.cpp
                            src/Gui/ComboBox.cpp
                            src/Gui/Manager.cpp
                            src/LandType.cpp)
set_property(TARGET Jutchs_Shmup PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)

add_library(OpenGL STATIC IMPORTED)
set_property(TARGET OpenGL PROPERTY IMPORTED_LOCATION opengl32.lib)
target_link_libraries(Jutchs_Shmup PRIVATE OpenGL)

add_library(SFML_Main STATIC IMPORTED)
set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION ../extlibs/SFML/lib/sfml-main.lib)
set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION_DEBUG ../extlibs/SFML/lib/sfml-main-d.lib)
target_link_libraries(Jutchs_Shmup PRIVATE SFML_Main)

add_library(SFML_System SHARED IMPORTED)
set_property(TARGET SFML_System PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-system-2.dll)
set_property(TARGET SFML_System PROPERTY IMPORTED_LOCATION_DEBUG ../extlibs/SFML/bin/sfml-system-d-2.dll)
set_property(TARGET SFML_System PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-system.lib)
set_property(TARGET SFML_System PROPERTY IMPORTED_IMPLIB_DEBUG ../extlibs/SFML/lib/sfml-system-d.lib)
target_link_libraries(Jutchs_Shmup PRIVATE SFML_System)

add_library(SFML_Window SHARED IMPORTED)
set_property(TARGET SFML_Window PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-window-2.dll)
set_property(TARGET SFML_Window PROPERTY IMPORTED_LOCATION_DEBUG ../extlibs/SFML/bin/sfml-window-d-2.dll)
set_property(TARGET SFML_Window PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-window.lib)
set_property(TARGET SFML_Window PROPERTY IMPORTED_IMPLIB_DEBUG ../extlibs/SFML/lib/sfml-window-d.lib)
target_link_libraries(Jutchs_Shmup PRIVATE SFML_Window)

add_library(SFML_Graphics SHARED IMPORTED)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-graphics-2.dll)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_LOCATION_DEBUG ../extlibs/SFML/bin/sfml-graphics-d-2.dll)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-graphics.lib)
set_property(TARGET SFML_Graphics PROPERTY IMPORTED_IMPLIB_DEBUG ../extlibs/SFML/lib/sfml-graphics-d.lib)
target_link_libraries(Jutchs_Shmup PRIVATE SFML_Graphics)

add_library(SFML_Audio SHARED IMPORTED)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_LOCATION ../extlibs/SFML/bin/sfml-audio-2.dll)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_LOCATION_DEBUG ../extlibs/SFML/bin/sfml-audio-d-2.dll)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_IMPLIB ../extlibs/SFML/lib/sfml-audio.lib)
set_property(TARGET SFML_Audio PROPERTY IMPORTED_IMPLIB_DEBUG ../extlibs/SFML/lib/sfml-audio-d.lib)
target_link_libraries(Jutchs_Shmup PRIVATE SFML_Audio)
