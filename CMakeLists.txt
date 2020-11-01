cmake_minimum_required(VERSION 3.11)

project (n64-game-jam C)

set(game_src
    src/camera.h src/camera.c
    src/filesystem.h
    src/input.h
    src/level.h src/level.c
    src/player.h src/player.c
    src/rect.h src/rect.c
    src/renderer.h
    src/tile_set.h src/tile_set.c)

set(sdl_src
    src/sdl/sdl_filesystem.c
    src/sdl/sdl_input.h src/sdl/sdl_input.c
    src/sdl/sdl_main.c
    src/sdl/sdl_renderer.h src/sdl/sdl_renderer.c
)

add_executable(n64-game-jam ${game_src} ${sdl_src})
target_compile_definitions(n64-game-jam PUBLIC PLATFORM_DESKTOP)

set_property(
    SOURCE src/sdl/sdl_main.c
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    ASSET_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/assets/build_sdl")

if (MSVC)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
    find_package(sdl2 REQUIRED)
    find_package(sdl2_image REQUIRED)

    target_link_libraries(n64-game-jam sdl2::sdl2 sdl2_image::sdl2_image)
else()
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2-cmake-modules)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    target_link_libraries(n64-game-jam SDL2::Main SDL2::Image SDL2::TTF)
    target_link_libraries(n64-game-jam m)
endif()