#include "createprocess.hpp"
#include "pic.hpp"

struct CreateProcessTest : PICTest<> {};

TEST_F(CreateProcessTest, PositionIndependent) {
    wchar_t calc[](L"calc.exe");
    EXPECT_TRUE(pic(calc));
}