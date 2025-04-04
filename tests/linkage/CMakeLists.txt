# Test that position independent libraries can be linked in the following
# combinations:
# 1. Static PIC lib  -> Normal executable
# 2. Static PIC lib  -> PIC executable
# 3. Static PIC lib  -> Dynamic PIC lib
# 4. Dynamic PIC lib -> Normal executable
add_subdirectory(crt-runner)
add_subdirectory(pic)