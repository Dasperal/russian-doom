foreach(SUBDIR base doom heretic hexen)
    add_subdirectory("${SUBDIR}")
endforeach()

# Common library
add_library(Common STATIC EXCLUDE_FROM_ALL
    aes_prng.c          aes_prng.h
                        doomkeys.h
                        doomfeatures.h
                        doomtype.h
    d_event.c           d_event.h
    d_iwad.c            d_iwad.h
                        d_name.h
    d_loop.c            d_loop.h
    d_mode.c            d_mode.h
                        d_ticcmd.h
    deh_str.c           deh_str.h
                        g_sk_unm.h
    gusconf.c           gusconf.h
    i_cdmus.c           i_cdmus.h
                        icon.h
    i_controller.c      i_controller.h
    i_endoom.c          i_endoom.h
    i_input.c           i_input.h
    i_glob.c            i_glob.h
                        i_swap.h
    i_pcsound.c
    i_sdlsound.c
    i_sdlmusic.c
    i_oplmusic.c
    i_sound.c           i_sound.h
    i_system.c          i_system.h
    i_timer.c           i_timer.h
    i_video.c           i_video.h
    i_videohr.c         i_videohr.h
    i_winmusic.c        i_winmusic.h
                        jn.h
                        l_sk_unm.h
    midifile.c          midifile.h
    mus2mid.c           mus2mid.h
    m_argv.c            m_argv.h
    m_bbox.c            m_bbox.h
    m_cheat.c           m_cheat.h
    m_config.c          m_config.h
    m_misc.c            m_misc.h
    m_fixed.c           m_fixed.h
    net_client.c        net_client.h
    net_common.c        net_common.h
    net_dedicated.c     net_dedicated.h
                        net_defs.h
    net_gui.c           net_gui.h
    net_io.c            net_io.h
    net_loop.c          net_loop.h
    net_packet.c        net_packet.h
    net_query.c         net_query.h
    net_sdl.c           net_sdl.h
    net_server.c        net_server.h
    net_structrw.c      net_structrw.h
    os_compat.c         os_compat.h
    rd_keybinds.c       rd_keybinds.h
    rd_menu.c           rd_menu.h
                        rd_menu_control.h
    rd_migration.c      rd_migration.h
    rd_text.c           rd_text.h
    sha1.c              sha1.h
    memio.c             memio.h
    tables.c            tables.h
    v_diskicon.c        v_diskicon.h
    v_video.c           v_video.h
                        v_patch.h
    v_trans.c           v_trans.h
    w_checksum.c        w_checksum.h
    w_main.c            w_main.h
    w_wad.c             w_wad.h
    w_file.c            w_file.h
    w_file_stdc.c
    w_file_posix.c
    w_file_win32.c
    w_merge.c           w_merge.h
    z_zone.c            z_zone.h
)
if(WIN32 AND MSVC)
    target_sources(Common PRIVATE
        win_opendir.c       win_opendir.h
    )
endif()
target_include_directories(Common PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
)
target_common_settings(Common)
target_compile_definitions(Common PRIVATE
    "$<$<BOOL:${SampleRate_FOUND}>:HAVE_LIBSAMPLERATE>"
    "$<$<BOOL:${HAVE_MMAP}>:HAVE_MMAP>"
    "$<$<BOOL:${HAVE_DIRENT_H}>:HAVE_DIRENT_H>"
    "$<IF:$<BOOL:${HAVE_DECL_SSCANF_S}>,HAVE_DECL_SSCANF_S=1,HAVE_DECL_SSCANF_S=0>"
    PACKAGE_TARNAME="${PACKAGE_TARNAME}"
    PROGRAM_PREFIX="${PROGRAM_PREFIX}"
    PUBLIC
    "$<IF:$<BOOL:${HAVE_DECL_STRCASECMP}>,HAVE_DECL_STRCASECMP=1,HAVE_DECL_STRCASECMP=0>"
    "$<IF:$<BOOL:${HAVE_DECL_STRNCASECMP}>,HAVE_DECL_STRNCASECMP=1,HAVE_DECL_STRNCASECMP=0>"
)
target_link_libraries(Common PUBLIC
    "$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>"
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
    SDL2_net::SDL2_net
    textscreen
    pcsound
    opl
    miniz::miniz
    "$<$<BOOL:${SampleRate_FOUND}>:SampleRate::samplerate>"
    "$<$<BOOL:${WIN32}>:winmm>"
)

# Create executables
foreach(MODULE ${COMPILE_MODULES})
    if(WIN32)
        add_library("${PROGRAM_PREFIX}${MODULE}" SHARED EXCLUDE_FROM_ALL
            i_main.c
            "${PROJECT_SOURCE_DIR}/data/icons/${MODULE}_icon.c"
        )
        set_target_properties("${PROGRAM_PREFIX}${MODULE}" PROPERTIES
            PREFIX ""
        )
        target_link_options("${PROGRAM_PREFIX}${MODULE}" PRIVATE
            "$<$<AND:$<C_COMPILER_ID:GNU>,$<STREQUAL:${ARCH},x86>>:-static-libgcc>"
        )

        # .exe
        add_executable("${PROGRAM_PREFIX}${MODULE}-exe" WIN32
            win_launcher.c
            "${CMAKE_CURRENT_BINARY_DIR}/${MODULE}/resource.rc"
        )
        target_link_options("${PROGRAM_PREFIX}${MODULE}-exe" PRIVATE
            "$<$<BOOL:${MSVC}>:/MANIFEST:NO>"
            "$<IF:$<BOOL:${MSVC}>,/SUBSYSTEM:WINDOWS,-mwindows>"
        )
        target_common_settings("${PROGRAM_PREFIX}${MODULE}-exe")
        target_compile_definitions("${PROGRAM_PREFIX}${MODULE}-exe" PRIVATE
            "$<IF:$<BOOL:${HAVE_DECL_STRCASECMP}>,HAVE_DECL_STRCASECMP=1,HAVE_DECL_STRCASECMP=0>"
            "$<IF:$<BOOL:${HAVE_DECL_STRNCASECMP}>,HAVE_DECL_STRNCASECMP=1,HAVE_DECL_STRNCASECMP=0>"
        )
        target_link_libraries("${PROGRAM_PREFIX}${MODULE}-exe" PRIVATE
            "$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>"
            SDL2::SDL2
            "${PROGRAM_PREFIX}${MODULE}"
        )
        target_include_directories("${PROGRAM_PREFIX}${MODULE}-exe" PRIVATE
            "${PROJECT_BINARY_DIR}"
        )
        add_custom_command(TARGET "${PROGRAM_PREFIX}${MODULE}-exe" POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${PROGRAM_PREFIX}${MODULE}-exe>"
            "$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-exe>/${PROGRAM_PREFIX}${MODULE}.exe"
            COMMAND $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:${CMAKE_STRIP}>
            $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:"$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-exe>/${PROGRAM_PREFIX}${MODULE}.exe">
        )

        # .com
        add_executable("${PROGRAM_PREFIX}${MODULE}-com" WIN32
            win_launcher.c
            "${CMAKE_CURRENT_BINARY_DIR}/${MODULE}/resource.rc"
        )
        target_link_options("${PROGRAM_PREFIX}${MODULE}-com" PRIVATE
            "$<$<BOOL:${MSVC}>:/MANIFEST:NO>"
            "$<IF:$<BOOL:${MSVC}>,/SUBSYSTEM:CONSOLE,-mconsole>"
        )
        target_common_settings("${PROGRAM_PREFIX}${MODULE}-com")
        target_compile_definitions("${PROGRAM_PREFIX}${MODULE}-com" PRIVATE
            "$<IF:$<BOOL:${HAVE_DECL_STRCASECMP}>,HAVE_DECL_STRCASECMP=1,HAVE_DECL_STRCASECMP=0>"
            "$<IF:$<BOOL:${HAVE_DECL_STRNCASECMP}>,HAVE_DECL_STRNCASECMP=1,HAVE_DECL_STRNCASECMP=0>"
            HAVE_CONSOLE
        )
        target_link_libraries("${PROGRAM_PREFIX}${MODULE}-com" PRIVATE
            "$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>"
            SDL2::SDL2
            "${PROGRAM_PREFIX}${MODULE}"
        )
        target_include_directories("${PROGRAM_PREFIX}${MODULE}-com" PRIVATE
            "${PROJECT_BINARY_DIR}"
        )
        add_custom_command(TARGET "${PROGRAM_PREFIX}${MODULE}-com" POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:${PROGRAM_PREFIX}${MODULE}-com>"
            "$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-com>/${PROGRAM_PREFIX}${MODULE}.com"
            COMMAND $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:${CMAKE_STRIP}>
            $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:"$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-com>/${PROGRAM_PREFIX}${MODULE}.com">
        )
    else()
        add_executable("${PROGRAM_PREFIX}${MODULE}"
            i_main.c
            "${PROJECT_SOURCE_DIR}/data/icons/${MODULE}_icon.c"
        )
    endif()

    # Dehacked sources
    if(NOT "${MODULE}" STREQUAL "hexen")
        target_sources("${PROGRAM_PREFIX}${MODULE}" PRIVATE
                                deh_defs.h
            deh_io.c            deh_io.h
            deh_main.c          deh_main.h
            deh_mapping.c       deh_mapping.h
            deh_text.c
        )
    endif()

    target_common_settings("${PROGRAM_PREFIX}${MODULE}")
    target_link_libraries("${PROGRAM_PREFIX}${MODULE}" PRIVATE
        ${MODULE}
        Common
    )
    if(NOT RD_GIT_NO_HASH)
        add_dependencies("${PROGRAM_PREFIX}${MODULE}" revision_check)
    endif()

    add_dependencies("${PROGRAM_PREFIX}${MODULE}" "${MODULE}-wads" gus-patches shared_libs)
    if(UNIX AND Python3_Interpreter_FOUND)
        add_dependencies("${PROGRAM_PREFIX}${MODULE}" "man-${MODULE}-en" "completion-${MODULE}")
    endif()

    add_custom_command(TARGET "${PROGRAM_PREFIX}${MODULE}" POST_BUILD
        COMMAND $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:${CMAKE_STRIP}> $<$<OR:$<CONFIG:RELEASE>,$<CONFIG:MINSIZEREL>>:"$<TARGET_FILE:${PROGRAM_PREFIX}${MODULE}>">
    )

    # Applocal dlls
    if(WIN32 AND (Python3_Interpreter_FOUND OR CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
        AND NOT VCPKG_TOOLCHAIN
        AND NOT RD_USE_SELECTED_DLL_SET)
        applocal_dependencies("${PROGRAM_PREFIX}${MODULE}"
            "$<IF:$<CONFIG:DEBUG>,${APPLOCAL_SEARCH_PATH_DEBUG},${APPLOCAL_SEARCH_PATH_RELEASE}>")
    endif()

    # Tests
    set(test_cmd $<TARGET_FILE:${PROGRAM_PREFIX}${MODULE}$<$<BOOL:${WIN32}>:-exe>> -timedemo demo1 -nogui -nosound)
    if(RD_TEST_WITH_GDB)
        set(gdb_cmd gdb -ex run -ex quit --args)
    endif()
    add_test(NAME "${PROGRAM_PREFIX}${MODULE}"
        COMMAND ${gdb_cmd} ${test_cmd}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/test_data"
    )
    set_tests_properties("${PROGRAM_PREFIX}${MODULE}" PROPERTIES
        PASS_REGULAR_EXPRESSION "Насчитано [-]?[0-9]+ gametics в [-]?[0-9]+ realtics;Timed [-]?[0-9]+ gametics in [-]?[0-9]+ realtics"
        FAIL_REGULAR_EXPRESSION "SEGV"
        TIMEOUT 150
    )
endforeach()

# Applocal optional dlls
if(NOT RD_USE_SELECTED_DLL_SET)
    get_target_property(_opt_dll SDL2_mixer::SDL2_mixer OPTIONAL_DLLS)
    copy_files(SOURCE ${_opt_dll} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    unset(_opt_dll)
endif()

# Install rules
foreach(MODULE ${COMPILE_MODULES})
    set(_permissions PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    if(WIN32)
        install(FILES
            "$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-exe>/${PROGRAM_PREFIX}${MODULE}.exe"
            "$<TARGET_FILE_DIR:${PROGRAM_PREFIX}${MODULE}-com>/${PROGRAM_PREFIX}${MODULE}.com"
            $<$<NOT:$<BOOL:${RD_USE_SELECTED_DLL_SET}>>:$<TARGET_PROPERTY:SDL2_mixer::SDL2_mixer,OPTIONAL_DLLS>>
            DESTINATION $<IF:$<BOOL:${WIN32}>,.,${CMAKE_INSTALL_BINDIR}>
            ${_permissions}
        )
        unset(_permissions)
    endif()
    install(TARGETS "${PROGRAM_PREFIX}${MODULE}" RUNTIME
        DESTINATION $<IF:$<BOOL:${WIN32}>,.,${CMAKE_INSTALL_BINDIR}>
        ${_permissions}
    )
    if(WIN32 AND (Python3_Interpreter_FOUND OR CMAKE_VERSION VERSION_GREATER_EQUAL 3.16)
        AND NOT VCPKG_TOOLCHAIN
        AND NOT RD_USE_SELECTED_DLL_SET)
        applocal_install_dependencies("${PROGRAM_PREFIX}${MODULE}"
            "$<IF:$<CONFIG:DEBUG>,${APPLOCAL_SEARCH_PATH_DEBUG},${APPLOCAL_SEARCH_PATH_RELEASE}>" "${MODULE}")
    endif()
endforeach()

install(FILES "${PROJECT_SOURCE_DIR}/LICENSE.txt"
    RENAME "GPL.txt"
    DESTINATION $<IF:$<BOOL:${WIN32}>,.,${CMAKE_INSTALL_DATAROOTDIR}/doc/${PACKAGE_TARNAME}>
)

# Source groups
source_group("Opl" REGULAR_EXPRESSION "^${PROJECT_SOURCE_DIR}/opl/.+")
source_group("Pcsound" REGULAR_EXPRESSION "^${PROJECT_SOURCE_DIR}/pcsound/.+")
source_group("Textscreen" REGULAR_EXPRESSION "^${PROJECT_SOURCE_DIR}/textscreen/.+")
source_group("Common sources" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/.+")
source_group("Doom" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/doom/.+")
source_group("Heretic" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/heretic/.+")
source_group("Hexen" REGULAR_EXPRESSION "^${CMAKE_CURRENT_SOURCE_DIR}/hexen/.+")
