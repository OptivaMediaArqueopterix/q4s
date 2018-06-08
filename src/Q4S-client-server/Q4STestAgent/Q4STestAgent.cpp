// Q4STestAgent.cpp : Defines the entry point for the console application.
//
#include "gtest/gtest.h"

#include "..\Q4SAgent\Q4SAgentStateManager.h"

TEST(setup_test_case, testWillPass)
{
 EXPECT_EQ(42, 42);
}

TEST(setup_test_case, testQ4SAgentStateManager)
{
    bool ok = true;

    Q4SAgentStateManager q4SAgentStateManager;

    ok &= q4SAgentStateManager.init();

    EXPECT_EQ(ok, true);
}