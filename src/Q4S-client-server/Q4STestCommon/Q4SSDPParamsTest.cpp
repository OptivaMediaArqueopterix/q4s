#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SSDPParams.h"

TEST(Q4SSDPParams_test_case, testQ4SSDPParamsOperaterEqual)
{
	Q4SSDPParams params;
	params.qosLevelUp = 1;
	params.qosLevelDown = 3;
	params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	params.alertPause = 25000;
	params.recoveryPause = 40000;
	params.latency = 6;
	params.jitterUp = 5;
	params.jitterDown = 5;
	params.bandWidthUp = 4000;
	params.bandWidthDown = 4000;
	params.packetLossUp = 0.3f;
	params.packetLossDown = 0.3f;
	params.procedure.negotiationTimeBetweenPingsUplink = 10;
	params.procedure.negotiationTimeBetweenPingsDownlink = 11;
	params.procedure.continuityTimeBetweenPingsUplink = 12;
	params.procedure.continuityTimeBetweenPingsDownlink = 13;
	params.procedure.bandwidthTime = 500;
	params.procedure.windowSizeLatencyCalcUplink = 40;
	params.procedure.windowSizeLatencyCalcDownlink = 41;
	params.procedure.windowSizePacketLossCalcUplink = 42;
	params.procedure.windowSizePacketLossCalcDownlink = 43;

	Q4SSDPParams paramsEqual;
	paramsEqual.qosLevelUp = 1;
	paramsEqual.qosLevelDown = 3;
	paramsEqual.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	paramsEqual.alertPause = 25000;
	paramsEqual.recoveryPause = 40000;
	paramsEqual.latency = 6;
	paramsEqual.jitterUp = 5;
	paramsEqual.jitterDown = 5;
	paramsEqual.bandWidthUp = 4000;
	paramsEqual.bandWidthDown = 4000;
	paramsEqual.packetLossUp = 0.3f;
	paramsEqual.packetLossDown = 0.3f;
	paramsEqual.procedure.negotiationTimeBetweenPingsUplink = 10;
	paramsEqual.procedure.negotiationTimeBetweenPingsDownlink = 11;
	paramsEqual.procedure.continuityTimeBetweenPingsUplink = 12;
	paramsEqual.procedure.continuityTimeBetweenPingsDownlink = 13;
	paramsEqual.procedure.bandwidthTime = 500;
	paramsEqual.procedure.windowSizeLatencyCalcUplink = 40;
	paramsEqual.procedure.windowSizeLatencyCalcDownlink = 41;
	paramsEqual.procedure.windowSizePacketLossCalcUplink = 42;
	paramsEqual.procedure.windowSizePacketLossCalcDownlink = 43;

	EXPECT_TRUE(params == paramsEqual);

	Q4SSDPParams paramsDistint;
	paramsDistint.qosLevelUp = 2;
	paramsDistint.qosLevelDown = 4;
	paramsDistint.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_REACTIVE;
	paramsDistint.alertPause = 2500;
	paramsDistint.recoveryPause = 400000;
	paramsDistint.latency = 60;
	paramsDistint.jitterUp = 50;
	paramsDistint.jitterDown = 50;
	paramsDistint.bandWidthUp = 400;
	paramsDistint.bandWidthDown = 400;
	paramsDistint.packetLossUp = 0.03f;
	paramsDistint.packetLossDown = 0.03f;

	EXPECT_FALSE(params == paramsDistint);

	Q4SSDPParams paramsAlmostEqual;
	paramsAlmostEqual.qosLevelUp = 1;
	paramsAlmostEqual.qosLevelDown = 3;
	paramsAlmostEqual.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	paramsAlmostEqual.alertPause = 25000;
	paramsAlmostEqual.recoveryPause = 40000;
	paramsAlmostEqual.latency = 6;
	paramsAlmostEqual.jitterUp = 5;
	paramsAlmostEqual.jitterDown = 6;
	paramsAlmostEqual.bandWidthUp = 4000;
	paramsAlmostEqual.bandWidthDown = 4000;
	paramsAlmostEqual.packetLossUp = 0.3f;
	paramsAlmostEqual.packetLossDown = 0.3f;

	EXPECT_FALSE(params == paramsAlmostEqual);
}

TEST(Q4SSDPParams_test_case, testQ4SSDPProcedureOperaterEqual)
{
	Q4SSDPProcedure procedure;
	procedure.negotiationTimeBetweenPingsUplink = 10;
	procedure.negotiationTimeBetweenPingsDownlink = 12;
	procedure.continuityTimeBetweenPingsUplink = 13;
	procedure.continuityTimeBetweenPingsDownlink = 14;
	procedure.bandwidthTime = 500;
	procedure.windowSizeLatencyCalcUplink = 40;
	procedure.windowSizeLatencyCalcDownlink = 40;
	procedure.windowSizePacketLossCalcUplink = 40;
	procedure.windowSizePacketLossCalcDownlink = 40;

	Q4SSDPProcedure procedureEqual;
	procedureEqual.negotiationTimeBetweenPingsUplink = 10;
	procedureEqual.negotiationTimeBetweenPingsDownlink = 12;
	procedureEqual.continuityTimeBetweenPingsUplink = 13;
	procedureEqual.continuityTimeBetweenPingsDownlink = 14;
	procedureEqual.bandwidthTime = 500;
	procedureEqual.windowSizeLatencyCalcUplink = 40;
	procedureEqual.windowSizeLatencyCalcDownlink = 40;
	procedureEqual.windowSizePacketLossCalcUplink = 40;
	procedureEqual.windowSizePacketLossCalcDownlink = 40;

	EXPECT_TRUE(procedure == procedureEqual);

	Q4SSDPProcedure procedureDistint;
	procedureDistint.negotiationTimeBetweenPingsUplink = 1;
	procedureDistint.negotiationTimeBetweenPingsDownlink = 2;
	procedureDistint.continuityTimeBetweenPingsUplink = 3;
	procedureDistint.continuityTimeBetweenPingsDownlink = 4;
	procedureDistint.bandwidthTime = 5;
	procedureDistint.windowSizeLatencyCalcUplink = 6;
	procedureDistint.windowSizeLatencyCalcDownlink = 7;
	procedureDistint.windowSizePacketLossCalcUplink = 8;
	procedureDistint.windowSizePacketLossCalcDownlink = 9;

	EXPECT_FALSE(procedure == procedureDistint);

	Q4SSDPProcedure procedureAlmostEqual;
	procedureAlmostEqual.negotiationTimeBetweenPingsUplink = 11;
	procedureAlmostEqual.negotiationTimeBetweenPingsDownlink = 12;
	procedureAlmostEqual.continuityTimeBetweenPingsUplink = 13;
	procedureAlmostEqual.continuityTimeBetweenPingsDownlink = 14;
	procedureAlmostEqual.bandwidthTime = 500;
	procedureAlmostEqual.windowSizeLatencyCalcUplink = 40;
	procedureAlmostEqual.windowSizeLatencyCalcDownlink = 40;
	procedureAlmostEqual.windowSizePacketLossCalcUplink = 40;
	procedureAlmostEqual.windowSizePacketLossCalcDownlink = 40;

	EXPECT_FALSE(procedure == procedureAlmostEqual);
}