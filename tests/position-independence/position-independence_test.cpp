#include "position-independence.hpp"
#include "pic.hpp"

struct PositionIndependenceTest : PICTest<> {};

TEST_F(PositionIndependenceTest, PICStable) {
    EXPECT_EQ(EXIT_SUCCESS, pic());
}
