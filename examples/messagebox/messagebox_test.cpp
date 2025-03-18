#include "messagebox.hpp"
#include "pic.hpp"

struct MessageBoxTest : PICTest<> {};

TEST_F(MessageBoxTest, PositionIndependent) {
    EXPECT_EQ(IDOK, pic());
}