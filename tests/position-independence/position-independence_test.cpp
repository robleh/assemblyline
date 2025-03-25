#include "position-independence.hpp"
#include "pic.hpp"

struct PositionIndependenceTest : PICTest<> {};

TEST_F(PositionIndependenceTest, PICStable) {
    ASSERT_NO_THROW(pic());
}
