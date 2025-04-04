add_executable(crt-runner main.cpp)
target_link_libraries(crt-runner PRIVATE al-test-lib al-test-lib-dll)

add_custom_command(
    TARGET crt-runner
    POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:crt-runner> $<TARGET_RUNTIME_DLLS:crt-runner>
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
