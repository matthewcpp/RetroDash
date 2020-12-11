cmake_minimum_required(VERSION 3.11)

project (RetroDash C)

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(EMSCRIPTEN TRUE)
endif()

set(game_src
    src/animation_player.h src/animation_player.c
    src/audio.h
    src/background.h src/background.c
    src/camera.h src/camera.c
    src/entity.h
    src/filesystem.h
    src/font_private.h src/font.c
    src/game.h src/game.c
    src/input.h
    src/intersection.h src/intersection.c
    src/level.h src/level.c
    src/particles.h src/particles.c
    src/player.h src/player.c
    src/rect.h src/rect.c
    src/renderer.h
    src/teleport.h src/teleport.c
    src/tile_collision.h src/tile_collision.c
    src/tile_set.h src/tile_set.c

    src/state/attempt_dialog.h src/state/attempt_dialog.c
    src/state/dialog.h src/state/dialog.c
    src/state/level_select.h src/state/level_select.c
    src/state/pause_dialog.h src/state/pause_dialog.c
    src/state/playing_base.h src/state/playing_base.c
    src/state/playing.h src/state/playing.c
    src/state/screen_util.h src/state/screen_util.c
    src/state/settings.h src/state/settings.c
    src/state/title.h src/state/title.c
    src/state/tutorial.h src/state/tutorial.c)

set(sdl_src
    src/sdl/sdl_audio.h src/sdl/sdl_audio.c
    src/sdl/sdl_filesystem.c
    src/sdl/sdl_font.c
    src/sdl/sdl_input.h src/sdl/sdl_input.c
    src/sdl/sdl_renderer.h src/sdl/sdl_renderer.c)

if (EMSCRIPTEN)
    set(platform_src src/sdl/sdl_emscripten_main.c)
else()
    set(platform_src src/sdl/sdl_desktop_main.c)
endif()

add_executable(RetroDash ${game_src} ${sdl_src} ${platform_src})


if(EMSCRIPTEN)
    set(USE_FLAGS "-s USE_SDL=2 -s USE_SDL_IMAGE=2  -s SDL2_IMAGE_FORMATS='[\"png\"]' -s USE_SDL_MIXER=2")
    # enable debug stack traces by adding -g below
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/tools/emscripten_shell.html")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set_target_properties(RetroDash PROPERTIES LINK_FLAGS "--preload-file build_sdl")

    set_property(
            SOURCE src/sdl/sdl_emscripten_main.c
            APPEND
            PROPERTY COMPILE_DEFINITIONS
            ASSET_DIRECTORY="build_sdl")

else()
    target_compile_definitions(RetroDash PUBLIC PLATFORM_DESKTOP)

    set_property(
            SOURCE src/sdl/sdl_desktop_main.c
            APPEND
            PROPERTY COMPILE_DEFINITIONS
            ASSET_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/assets/build_sdl")

    if (MSVC)
        include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
        find_package(sdl2 REQUIRED)
        find_package(sdl2_image REQUIRED)
        find_package(SDL2_mixer REQUIRED)

        target_link_libraries(RetroDash sdl2::sdl2 sdl2_image::sdl2_image sdl2_mixer::sdl2_mixer)
    else()
        list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2-cmake-modules)
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_mixer REQUIRED)

        target_link_libraries(RetroDash SDL2::Main SDL2::Image SDL2::Mixer)
        target_link_libraries(RetroDash m)
    endif()
endif()