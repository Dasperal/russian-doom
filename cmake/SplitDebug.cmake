if(APPLE)
    find_program(DSYMUTIL_executable NAMES dsymutil llvm-dsymutil)
    if(NOT DSYMUTIL_executable)
        message(FATAL_ERROR "Split debug info requires dsymutil on macOS")
    endif()
else()
    find_program(OBJCOPY_executable NAMES llvm-objcopy objcopy)
    if(NOT OBJCOPY_executable)
        message(FATAL_ERROR "Split debug info requires objcopy or llvm-objcopy")
    endif()
    if(NOT CMAKE_STRIP)
        message(FATAL_ERROR "Split debug info requires a strip tool")
    endif()
endif()

function(attach_split_debug target)
    if(APPLE)
        set(debug_path "$<TARGET_FILE:${target}>.dSYM")
        add_custom_command(TARGET "${target}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${debug_path}"
            COMMAND "${DSYMUTIL_executable}" "$<TARGET_FILE:${target}>" -o "${debug_path}"
            COMMAND "${CMAKE_STRIP}" -S "$<TARGET_FILE:${target}>"
        )
    else()
        set(debug_path "$<TARGET_FILE:${target}>.debug")
        add_custom_command(TARGET "${target}" POST_BUILD
            COMMAND "${OBJCOPY_executable}" --only-keep-debug "$<TARGET_FILE:${target}>" "${debug_path}"
            COMMAND "${CMAKE_STRIP}" --strip-unneeded "$<TARGET_FILE:${target}>"
            COMMAND "${OBJCOPY_executable}" --add-gnu-debuglink="${debug_path}" "$<TARGET_FILE:${target}>"
        )
    endif()
endfunction()
