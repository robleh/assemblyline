add_pic_library(al-test-dll SHARED entry.cpp)
target_link_libraries(al-test-dll PRIVATE al-test-lib)
target_include_directories(al-test-dll INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Static Lib should contain a log message function that takes a parameter
# DLL should link to static lib and log "dll log success"
# Exe should link to both and call the PIC export  n
