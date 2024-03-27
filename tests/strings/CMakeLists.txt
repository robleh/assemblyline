add_pic(strings entry.cpp strings.hpp)
target_link_libraries(al-test PRIVATE strings)
target_sources(al-test PRIVATE strings_test.cpp)
