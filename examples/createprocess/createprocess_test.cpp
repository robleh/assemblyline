#include "createprocess.hpp"
#include <al/gtest/pic.hpp>

struct CreateProcessTest : public PICTest<CreateProcessTest> {
    inline static std::string_view path = CREATEPROCESS_PIC_PATH;
    inline static unsigned long permissions = PAGE_EXECUTE_READWRITE;
    createprocess_t m_pic = nullptr;
};

TEST_F(CreateProcessTest, PositionIndependent) {
    wchar_t calc[](L"calc.exe");
    EXPECT_TRUE(m_pic(calc));
}