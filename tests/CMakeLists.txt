# google/googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY
        https://github.com/google/googletest
    GIT_TAG
        v1.14.0
)

set(INSTALL_GTEST OFF)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_library(al-gtest INTERFACE)
add_library(al::gtest ALIAS al-gtest)

target_sources(
    al-gtest
    INTERFACE
        FILE_SET
            al_gtest_header
        TYPE
            HEADERS
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/pic.hpp
)

add_executable(al-test)
add_executable(al::test ALIAS al-test)

target_link_libraries(al-test PRIVATE GTest::gtest_main al::gtest al::al)
target_include_directories(al-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (AL_TEST_C)
    add_subdirectory("c")
endif()

if (AL_TEST_POSITION_INDEPENDENCE)
add_subdirectory("position-independence")
endif()

if (AL_TEST_LINKAGE)
    add_subdirectory("linkage")
endif()

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
