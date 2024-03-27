# Tests
Assembly Line includes a small GTest extension for testing PIC.

## Example
Using the messagebox target from the [examples directory](../examples) for
demonstration.

Link the test executable with a PIC target. This will expose the PIC's public
header and path to the tester.

```CMake
target_link_libraries(al-test PRIVATE messagebox)
target_sources(al-test PRIVATE messagebox_test.cpp)
```

Then inherit from the `PICTest` class and set the path to your PIC using the
`PIC_PATH` macro. This will create a GTest fixture that reads the PIC from
disk, allocates it in a buffer with the given permissions, and casts it to
type provided. Now, all tests can execute simply execute the PIC as function.

```C++
#include "messagebox.hpp"
#include <al/test.hpp>

struct MessageBoxTest : public PICTest<MessageBoxTest> {
    inline static std::string_view path = MESSAGEBOX_PIC_PATH;
    inline static unsigned long permissions = PAGE_EXECUTE_READWRITE;
    messagebox_t m_pic = nullptr;
};

TEST_F(MessageBoxTest, PositionIndependent) {
    ASSERT_EQ(EXIT_SUCCESS, m_pic());
}
```

Check out the [GoogleTest primer](https://github.com/google/googletest/blob/main/docs/primer.md)
for more information.