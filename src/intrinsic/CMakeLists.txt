add_library(al-intrinsic STATIC)
add_library(al::intrinsic ALIAS al-intrinsic)
set_target_properties(al-intrinsic PROPERTIES EXPORT_NAME intrinsic)

target_sources(
    al-intrinsic
    PRIVATE
        intrinsic.cpp
    PUBLIC
        FILE_SET al_intrinsic_header TYPE HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include/al
        FILES
            ${PROJECT_SOURCE_DIR}/include/al/intrinsic.hpp
)

target_link_libraries(al-intrinsic PRIVATE al::compiler)
target_include_directories(
    al-intrinsic
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (INSTALL_AL)
    install(
        TARGETS
            al-intrinsic
        EXPORT
            al-targets
        ARCHIVE DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
        FILE_SET al_intrinsic_header DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
