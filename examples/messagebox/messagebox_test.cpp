#include "messagebox.hpp"
#include <gtest/gtest.h>
#include <windows.h>
import run;

TEST(MessageBoxTest, PositionIndependent) {
    al::tools::run<messagebox_t> pic(MESSAGEBOX_PIC_PATH);
    EXPECT_EQ(IDOK, pic());
}