# Allow PIC GTests to be toggled
# Allow GTest to be fetched for examples or other targets independently
# If GTest is not enabled do not link al::al to al-test, probably set in the source directory
#
# AL_TESTS run Assembly Line unit and PIC tests
# AL_GTEST Fetch GTest
# AL_TEST_POSITION_INDEPENDENCE...
# AL_UNIT_TESTS
add_executable(al-test)
add_executable(al::test ALIAS al-test)

target_link_libraries(al-test PRIVATE GTest::gtest_main al::run)
target_include_directories(al-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (AL_TEST_C)
    add_subdirectory("c")
endif()

if (AL_TEST_PIC)
    add_subdirectory("pic")
endif()

if (AL_TEST_LINK)
    add_subdirectory("link")
endif()

if(AL_GTEST)
    gtest_discover_tests(al-test)
endif()
