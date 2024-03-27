add_pic(messagebox entry.cpp messagebox.hpp)

if (AL_TESTS)
    target_link_libraries(al-test PRIVATE messagebox)
    target_sources(al-test PRIVATE messagebox_test.cpp)
endif()
