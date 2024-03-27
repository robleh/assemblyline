add_pic(imports entry.cpp imports.hpp)
target_link_libraries(al-test PRIVATE imports)
target_sources(al-test PRIVATE imports_test.cpp)
