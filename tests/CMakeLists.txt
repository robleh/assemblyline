# google/googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY
        https://github.com/google/googletest
    GIT_TAG
        v1.14.0
)

set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set_target_properties(gtest PROPERTIES STATIC_LIBRARY_OPTIONS /nologo)
set_target_properties(gtest_main PROPERTIES STATIC_LIBRARY_OPTIONS /nologo)
target_compile_options(gtest PUBLIC /nologo)

add_library(al-gtest INTERFACE)
add_library(al::gtest ALIAS al-gtest)

target_sources(
    al-gtest
    INTERFACE
        FILE_SET
            al_gtest_header
        TYPE
            HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include
        FILES
            ${PROJECT_SOURCE_DIR}/include/al/gtest/pic.hpp
)

add_executable(al-test)
add_executable(al::test ALIAS al-test)
set_target_properties(al-test PROPERTIES STATIC_LIBRARY_OPTIONS /nologo)

target_link_libraries(al-test PRIVATE GTest::gtest_main al::gtest)

add_subdirectory("imports")
add_subdirectory("strings")
add_subdirectory("cast")
gtest_discover_tests(al-test)

if (INSTALL_AL)
    install(
        TARGETS
            al-gtest
        EXPORT
            al-targets
        ARCHIVE DESTINATION
            ${CMAKE_INSTALL_LIBDIR}
        FILE_SET al_gtest_header DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()
