add_pic_executable(pic entry.cpp)

target_link_libraries(al-test PRIVATE pic.pic)
target_sources(al-test PRIVATE "pic_test.cpp")