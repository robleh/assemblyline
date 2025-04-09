add_pic_executable(createprocess entry.cpp)
add_executable(al::examples::createprocess ALIAS createprocess)

target_link_libraries(al-examples-gtest PRIVATE createprocess.pic)
target_sources(al-examples-gtest PRIVATE createprocess_test.cpp)