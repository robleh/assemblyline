add_pic_library(al-test-lib STATIC test.cpp)
target_include_directories(al-test-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_pic_executable(al-test-lib-exe exe-entry.cpp)
target_link_libraries(al-test-lib-exe PRIVATE al-test-lib)

add_pic_library(al-test-lib-dll SHARED dll-entry.cpp)
target_link_libraries(al-test-lib-dll PRIVATE al-test-lib)

# Static Lib should contain a log message function that takes a parameter
# DLL should link to static lib and log "dll log success"
# Exe should link to both and call the PIC export  n
