al_mask_flags()
add_subdirectory("intrinsic")

add_library(al STATIC)
add_library(al::al ALIAS al)
set_target_properties(al PROPERTIES EXPORT_NAME al)

target_link_libraries(al PRIVATE al::compiler)

target_include_directories(
    al
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (AL_ASM)
    add_subdirectory("x64")
endif()

target_sources(
    al
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/al.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/import.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/hash.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/prng.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/string.cppm
    PUBLIC
        FILE_SET al_header TYPE HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include/al
        FILES
            ${PROJECT_SOURCE_DIR}/include/al/al.hpp
)

target_sources(
    al-test
    PRIVATE
        import_test.cpp
        hash_test.cpp
)

if (INSTALL_AL)
    install(
        TARGETS
            al
        EXPORT
            al-targets
        ARCHIVE DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
        FILE_SET CXX_MODULES DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET al_header DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
