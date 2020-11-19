cmake_minimum_required(VERSION 3.11)

project (n64-game-jam C)

set(game_src
    src/animation_player.h src/animation_player.c
    src/audio.h
    src/camera.h src/camera.c
    src/entity.h
    src/filesystem.h
    src/game.h src/game.c
    src/input.h
    src/intersection.h src/intersection.c
    src/level.h src/level.c
    src/particles.h src/particles.c
    src/player.h src/player.c
    src/rect.h src/rect.c
    src/renderer.h
    src/tile_collision.h src/tile_collision.c
    src/tile_set.h src/tile_set.c

    src/state/playing.h src/state/playing.c
    src/state/title.h src/state/title.c)

set(sdl_src
    src/sdl/sdl_audio.h src/sdl/sdl_audio.c
    src/sdl/sdl_filesystem.c
    src/sdl/sdl_input.h src/sdl/sdl_input.c
    src/sdl/sdl_main.c
    src/sdl/sdl_renderer.h src/sdl/sdl_renderer.c)

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
    find_package(SDL2_mixer REQUIRED)

    target_link_libraries(n64-game-jam SDL2::Main SDL2::Image SDL2::Mixer)
    target_link_libraries(n64-game-jam m)
endif()