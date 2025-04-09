add_library(al-run STATIC)
add_library(al::run ALIAS al-run)

target_sources(
    al-run
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/run.cppm
)