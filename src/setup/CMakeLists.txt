add_library(setup STATIC
            compatibility.c     compatibility.h
            display.c           display.h
            joystick.c          joystick.h
            keyboard.c          keyboard.h
            mainmenu.c
            mode.c              mode.h
            mouse.c             mouse.h
            multiplayer.c       multiplayer.h
            sound.c             sound.h
            execute.c           execute.h
            txt_joyaxis.c       txt_joyaxis.h
            txt_joybinput.c     txt_joybinput.h
            txt_keyinput.c      txt_keyinput.h
            txt_mouseinput.c    txt_mouseinput.h
)

target_include_directories(setup PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(setup
    textscreen
    SDL2::SDL2
    SDL2::mixer
)

# [Dasperal] Disable clang warnings for windows-1251 strings
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(setup PRIVATE -Wno-invalid-source-encoding)
endif()