add_pic(createprocess entry.cpp createprocess.hpp)

if (AL_TESTS)
    target_link_libraries(al-test PRIVATE createprocess)
    target_sources(al-test PRIVATE createprocess_test.cpp)
endif()
