add_executable(al-examples-gtest)
add_executable(al::examples::gtest ALIAS al-examples-gtest)
target_link_libraries(al-examples-gtest PRIVATE GTest::gtest_main al::run)
gtest_discover_tests(al-examples-gtest)