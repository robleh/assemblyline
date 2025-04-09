add_library(al-run STATIC)
add_library(al::run ALIAS al-run)

target_sources(
    al-run
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/run.cppm
)

# Mismatch between exception directives in al.pcm and run.pcm are throwing a
# Clang compilation error. Tried to fix by specifying
# -Wno-module-file-config-mismatch to no avail.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    target_compile_options(al-run PUBLIC -fno-exceptions)
endif()