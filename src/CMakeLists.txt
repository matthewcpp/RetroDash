set(src_files
    game.h game.c)

create_game(
    TARGET RetroDash
    EXTRA_LIBS fw64_components
    SOURCES ${src_files})