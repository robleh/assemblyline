#include "strings.hpp"
#include <al/gtest/pic.hpp>

struct StringsTest : PICTest<> {};

TEST_F(StringsTest, PICStable) {
    ASSERT_NO_THROW(pic());
}

TEST_F(StringsTest, CharStackStrings) {
    auto results = pic();
    EXPECT_STREQ("444", results->stack.four);
    EXPECT_STREQ("8888888", results->stack.eight);
    EXPECT_STREQ("161616161616161", results->stack.sixteen);
    EXPECT_STREQ("3232323232323232323232323323232", results->stack.thirty_two);
}

TEST_F(StringsTest, WideStackStrings) {
    auto results = pic();
    EXPECT_STREQ(L"4", results->stack.wfour);
    EXPECT_STREQ(L"888", results->stack.weight);
    EXPECT_STREQ(L"1616161", results->stack.wsixteen);
    EXPECT_STREQ(L"32323232323232", results->stack.wthirty_two);
}

TEST_F(StringsTest, CharLiteralStrings) {
    auto results = pic();
    EXPECT_STREQ("444", results->literal.four);
    EXPECT_STREQ("8888888", results->literal.eight);
    EXPECT_STREQ("161616161616161", results->literal.sixteen);
    EXPECT_STREQ("3232323232323232323232323323232", results->literal.thirty_two);
}

TEST_F(StringsTest, WideLiteralStrings) {
    auto results = pic();
    EXPECT_STREQ(L"4", results->literal.wfour);
    EXPECT_STREQ(L"888", results->literal.weight);
    EXPECT_STREQ(L"1616161", results->literal.wsixteen);
    EXPECT_STREQ(L"32323232323232", results->literal.wthirty_two);
}
