cmake_minimum_required(VERSION 3.15)

project(Jutchs_Shmup VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(GCC_LIKE_CXX $<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>)
set(MSVC_CXX $<COMPILE_LANG_AND_ID:CXX,MSVC>)

add_executable(${PROJECT_NAME} WIN32)

target_sources(${PROJECT_NAME} PRIVATE src/main.cpp 
                                       src/util.cpp
                                       src/GameState.cpp 
                                       src/LanguageManager.cpp
                                       src/AssetManager.cpp 
                                       src/EntityManager.cpp 
                                       src/Bullet.cpp
                                       src/AnimatedParticle.cpp 
                                       src/Bomb.cpp)

target_sources(${PROJECT_NAME} PRIVATE src/Airplane/ShootComponent.cpp 
                                       src/Airplane/MoveComponent.cpp 
                                       src/Airplane/ShootControlComponent.cpp 
                                       src/Airplane/Flags.cpp
                                       src/Airplane/BombComponent.cpp
                                       src/Airplane/DeathEffect.cpp)

target_sources(${PROJECT_NAME} PRIVATE src/Gui/Panel.cpp 
                                       src/Gui/Button.cpp 
                                       src/Gui/HorizontalSlider.cpp 
                                       src/Gui/ComboBox.cpp
                                       src/Gui/Manager.cpp)

target_sources(${PROJECT_NAME} PRIVATE src/Land/Type.cpp src/Land/Manager.cpp)     

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>DLL)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<${GCC_LIKE_CXX}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>
    $<${MSVC_CXX}:-W3>
)

set(CMAKE_INSTALL_BINDIR ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} TYPE RUNTIME)

target_sources(${PROJECT_NAME} PRIVATE system/appicon.rc)     


set(SFML_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/SFML)

target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_DIR}/include)

if(CMAKE_HOST_WIN32)
    add_library(SFML_Main STATIC IMPORTED)
    set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION       ${SFML_DIR}/windows/lib/sfml-main.lib)
    set_property(TARGET SFML_Main PROPERTY IMPORTED_LOCATION_DEBUG ${SFML_DIR}/windows/lib/sfml-main-d.lib)
    target_link_libraries(${PROJECT_NAME} SFML_Main)
endif()

function(add_sfml_module MODULE_NAME)
    add_library(${MODULE_NAME} SHARED IMPORTED)

    if(CMAKE_HOST_WIN32)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION       
            ${SFML_DIR}/windows/bin/sfml-${MODULE_NAME}-2.dll)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION_DEBUG 
            ${SFML_DIR}/windows/bin/sfml-${MODULE_NAME}-d-2.dll)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_IMPLIB         
            ${SFML_DIR}/windows/lib/sfml-${MODULE_NAME}.lib)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_IMPLIB_DEBUG   
            ${SFML_DIR}/windows/lib/sfml-${MODULE_NAME}-d.lib)
    elseif(CMAKE_HOST_APPLE)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION 
            ${SFML_DIR}/macos/lib/libsfml-${MODULE_NAME}.2.5.1.dylib)
    elseif(CMAKE_HOST_UNIX)
        set_property(TARGET ${MODULE_NAME} PROPERTY IMPORTED_LOCATION       
            ${SFML_DIR}/linux/lib/libsfml-${MODULE_NAME}.so.2.5.1)
    endif()

    target_link_libraries(${PROJECT_NAME} ${MODULE_NAME})
    
    if(CMAKE_HOST_WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${SFML_DIR}/windows/bin/sfml-${MODULE_NAME}$<$<CONFIG:Debug>:-d>-2.dll 
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endif()

    if(CMAKE_HOST_WIN32)
        install(FILES ${SFML_DIR}/windows/bin/sfml-${MODULE_NAME}$<$<CONFIG:Debug>:-d>-2.dll TYPE BIN)
    elseif(CMAKE_HOST_APPLE)
        install(FILES ${SFML_DIR}/macos/lib/libsfml-${MODULE_NAME}.2.5.1.dylib TYPE LIB)
    elseif(CMAKE_HOST_UNIX)
        install(FILES ${SFML_DIR}/linux/lib/libsfml-${MODULE_NAME}.so.2.5.1 TYPE LIB)
    endif()
endfunction()

add_sfml_module(system)
add_sfml_module(window)
add_sfml_module(graphics)
add_sfml_module(audio)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${PROJECT_NAME})
