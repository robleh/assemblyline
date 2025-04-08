#include "pic.hpp"
#include <gtest/gtest.h>
import run;

TEST(PositionIndependenceTest, PICStable) {
    al::run<decltype(&entry)> pic(POSITION_INDEPENDENCE_PIC_PATH);
    EXPECT_EQ(EXIT_SUCCESS, pic());
}
