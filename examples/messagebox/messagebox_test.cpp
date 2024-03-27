#include "messagebox.hpp"
#include <al/gtest/pic.hpp>

struct MessageBoxTest : public PICTest<MessageBoxTest> {
    inline static std::string_view path = MESSAGEBOX_PIC_PATH;
    inline static unsigned long permissions = PAGE_EXECUTE_READWRITE;
    messagebox_t m_pic = nullptr;
};

TEST_F(MessageBoxTest, PositionIndependent) {
    EXPECT_EQ(IDOK, m_pic(L"Hit OK to pass or Cancel to fail the test case.", L"Assembly Line PIC Test"));
}