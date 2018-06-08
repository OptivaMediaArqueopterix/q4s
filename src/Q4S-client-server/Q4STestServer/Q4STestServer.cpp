// Q4STestServer.cpp : Defines the entry point for the console application.
//
#include "gtest/gtest.h"

#include "..\Q4SServer\Q4SServerStateManager.h"
#include "..\Q4SServer\Q4SServerProtocol.h"

TEST(setup_test_case, testWillPass)
{
 EXPECT_EQ(42, 42);
}

TEST(setup_test_case, testQ4SServerProtocol)
{
    bool ok = true;

    Q4SServerProtocol q4SServerProtocol;

    ok &= q4SServerProtocol.init();

    EXPECT_EQ(ok, true);
}

TEST(setup_test_case, DISABLED_testQ4SAgentStateManager)
{
    bool ok = true;

    Q4SServerStateManager q4SServerStateManager;

    ok &= q4SServerStateManager.init();

    EXPECT_EQ(ok, true);
}