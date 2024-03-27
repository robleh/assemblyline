mask_default_flags()

add_library(al STATIC)
add_library(al::al ALIAS al)

set_target_properties(al PROPERTIES EXPORT_NAME al)
set_target_properties(al PROPERTIES STATIC_LIBRARY_OPTIONS /nologo)
set_target_properties(al PROPERTIES EXPORT_COMPILE_COMMANDS ON)

target_link_libraries(
    al
    PUBLIC
        $<BUILD_INTERFACE:phnt_headers>
        al::compiler
)

target_include_directories(
    al
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(align_stack OBJECT x64/align.asm)
target_compile_options(align_stack PRIVATE /nologo)

target_sources(
    al
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/import.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pe/export.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/peb/peb.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/memcpy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/memcmp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/memmove.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/memset.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/strlen.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/intrinsic/wcslen.cpp
        $<TARGET_OBJECTS:align_stack>
    PUBLIC
        FILE_SET
            al_headers
        TYPE
            HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include
        FILES
            ${PROJECT_SOURCE_DIR}/include/al/al.hpp
            ${PROJECT_SOURCE_DIR}/include/al/hash.hpp
            ${PROJECT_SOURCE_DIR}/include/al/import.hpp
            ${PROJECT_SOURCE_DIR}/include/al/intrinsic.hpp
            ${PROJECT_SOURCE_DIR}/include/al/pe.hpp
            ${PROJECT_SOURCE_DIR}/include/al/peb.hpp
            ${PROJECT_SOURCE_DIR}/include/al/prng.hpp
            ${PROJECT_SOURCE_DIR}/include/al/string.hpp
)

if (INSTALL_AL)
    install(
        TARGETS
            al
        EXPORT
            al-targets
        ARCHIVE DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
        FILE_SET al_headers DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        FILES
            ${phnt_SOURCE_DIR}/phnt.h
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
