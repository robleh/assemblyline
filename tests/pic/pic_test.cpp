#include "pic.hpp"
#include <gtest/gtest.h>
import run;

TEST(PositionIndependenceTest, PICStable) {
    al::tools::run<decltype(&entry)> pic(POSITION_INDEPENDENCE_PIC_PATH);
    EXPECT_EQ(EXIT_SUCCESS, pic());
}
