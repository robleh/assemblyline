#include "createprocess.hpp"
#include <gtest/gtest.h>
import run;

TEST(CreateProcessTest, PositionIndependent) {
    al::tools::run<createprocess_t> pic(CREATEPROCESS_PIC_PATH);
    wchar_t cmd[](L"calc.exe");
    EXPECT_TRUE(pic(cmd));
}