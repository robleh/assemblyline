add_executable(al-test-runner main.cpp)
target_link_libraries(al-test-runner PRIVATE al-test-lib al-test-dll)

add_custom_command(
    TARGET al-test-runner
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:al-test-runner> $<TARGET_RUNTIME_DLLS:al-test-runner>
)

# Static Lib should contain a log message function that takes a parameter
# DLL should link to static lib and log "dll log success"
# Exe should link to both and call the PIC export  n

# 1. Static PIC lib  -> Normal executable
# 2. Static PIC lib  -> Dynamic PIC lib
# 3. Static PIC lib  -> PIC executable

# 4. Dynamic PIC lib -> Normal executable
# 5. Dynamic PIC lib -> PIC test

# 6. PIC executable  -> PIC test

# /link-tests
#   /runners
#   /pic
