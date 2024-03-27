add_pic(cast entry.cpp cast.hpp)
target_link_libraries(al-test PRIVATE cast)
target_sources(al-test PRIVATE cast_test.cpp)