// testFixture.cpp
#include "gtest/gtest.h"
TEST(setup_test_case, testWillPass)
{
 EXPECT_EQ(42, 42);
}
TEST(setup_test_case, testWillFail)
{
 EXPECT_EQ(42, 0);
}