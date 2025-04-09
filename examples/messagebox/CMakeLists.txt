add_pic_library(messagebox SHARED entry.cpp)
target_link_libraries(al-examples-gtest PRIVATE messagebox.pic)
target_sources(al-examples-gtest PRIVATE messagebox_test.cpp)
