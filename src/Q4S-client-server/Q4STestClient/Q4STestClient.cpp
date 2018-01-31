// Q4STestClient.cpp : Defines the entry point for the console application.
//
#include "gtest/gtest.h"

#include "..\Q4SClient\Q4SClientStateManager.h"
#include "..\Q4SCommon\EKey.h"

TEST(setup_test_case, testWillPass)
{
 EXPECT_EQ(42, 42);
}

TEST(setup_test_case, DISABLED_testQ4SClientProtocolInit)
{
    bool ok = true;

    Q4SClientProtocol q4SClientProtocol;

    ok &= q4SClientProtocol.init(20, 500);

    EXPECT_EQ(ok, true);
}

TEST(setup_test_case, DISABLED_testQ4SClientProtocolBegin)
{
    bool ok = true;

    Q4SClientProtocol q4SClientProtocol;

    ok &= q4SClientProtocol.init(20, 500);

    EXPECT_EQ(ok, true);

	Q4SSDPParams params;
    ok &= q4SClientProtocol.handshake(params);

    EXPECT_EQ(ok, true);
}

TEST(setup_test_case, testQ4SClientProtocolContinuity)
{
    bool ok = true;

    Q4SClientProtocol q4SClientProtocol;

    ok &= q4SClientProtocol.init(20, 500);

    EXPECT_EQ(ok, true);

	Q4SSDPParams params;
    ok &= q4SClientProtocol.handshake(params);

    EXPECT_EQ(ok, true);

	ok &= q4SClientProtocol.ready(0);
	if( ok )
	{
		Q4SMeasurementResult results;

		ok = q4SClientProtocol.negotiation(params, results);
	}

    EXPECT_EQ(ok, true);

	printf("Negotiation OK, launch GANY and press any key\n");
    while(!EKey_anyKey())
    {
    }

}

TEST(setup_test_case, DISABLED_testQ4SClientStateManager)
{
    bool ok = true;

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();

    EXPECT_EQ(ok, true);
}

