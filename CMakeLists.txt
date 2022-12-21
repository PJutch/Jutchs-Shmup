cmake_minimum_required(VERSION 3.15)

project(Jutchs_Shmup VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_LIKE_CXX $<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>)
set(MSVC_CXX $<COMPILE_LANG_AND_ID:CXX,MSVC>)

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
                            src/Land/Type.cpp
                            src/Land/Manager.cpp
                            src/Airplane/Flags.cpp
                            src/Bomb.cpp
                            src/Airplane/BombComponent.cpp)

set_property(TARGET Jutchs_Shmup PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
target_compile_options(Jutchs_Shmup PRIVATE
    $<${GCC_LIKE_CXX}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>
    $<${MSVC_CXX}:-W3>
)

set(SFML_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML)

target_include_directories(Jutchs_Shmup PRIVATE ${SFML_PATH}/include)

if(CMAKE_HOST_WIN32)
    add_library(SFML_Main STATIC IMPORTED)
    set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION       ${SFML_PATH}/windows/lib/sfml-main.lib)
    set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION_DEBUG ${SFML_PATH}/windows/lib/sfml-main-d.lib)
    target_link_libraries(Jutchs_Shmup SFML_Main)
endif()

macro(add_sfml_module MODULE_NAME)
    add_library(${MODULE_NAME} SHARED IMPORTED)

    if(CMAKE_HOST_WIN32)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION       
            ${SFML_PATH}/windows/bin/sfml-${MODULE_NAME}-2.dll)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION_DEBUG 
            ${SFML_PATH}/windows/bin/sfml-${MODULE_NAME}-d-2.dll)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_IMPLIB         
            ${SFML_PATH}/windows/lib/sfml-${MODULE_NAME}.lib)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_IMPLIB_DEBUG   
            ${SFML_PATH}/windows/lib/sfml-${MODULE_NAME}-d.lib)
    elseif(CMAKE_HOST_APPLE)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION 
            ${SFML_PATH}/macos/lib/libsfml-${MODULE_NAME}.2.5.1.dylib)
    elseif(CMAKE_HOST_UNIX)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION       
            ${SFML_PATH}/linux/lib/libsfml-${MODULE_NAME}.so.2.5.1)
    endif()


    target_link_libraries(Jutchs_Shmup ${MODULE_NAME})
    
    if(CMAKE_HOST_WIN32)
        add_custom_command(TARGET Jutchs_Shmup POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${SFML_PATH}/windows/bin/sfml-${MODULE_NAME}$<$<CONFIG:Debug>:-d>-2.dll 
            $<TARGET_FILE_DIR:Jutchs_Shmup>)
    endif()
endmacro()

add_sfml_module(system)
add_sfml_module(window)
add_sfml_module(graphics)
add_sfml_module(audio)

# assume Debug is run with changed working directory
if(NOT $<CONFIG:Debug>)
    add_custom_command(TARGET Jutchs_Shmup POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Jutchs_Shmup>/resources)
endif()
