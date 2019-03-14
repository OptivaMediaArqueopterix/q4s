#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SCommonProtocol.h"
#include "..\Q4SCommon\Q4SSDP.h"

TEST(Q4SSDP_test_case, testQ4SSDPWithDefaultParams)
{
	Q4SSDPParams params;
	params.qosLevelUp = 1;
	params.qosLevelDown = 3;
	params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	params.alertPause = 25000;
	params.recoveryPause = 40000;
	params.latency = 6;
	params.jitterUp = 5;
	params.jitterDown = 6;
	params.bandWidthUp = 4000;
	params.bandWidthDown = 3000;
	params.packetLossUp = 0.3f;
	params.packetLossDown = 0.3f;
	params.procedure.negotiationTimeBetweenPingsUplink = 10;
	params.procedure.negotiationTimeBetweenPingsDownlink = 12;
	params.procedure.continuityTimeBetweenPingsUplink = 13;
	params.procedure.continuityTimeBetweenPingsDownlink = 14;
	params.procedure.bandwidthTime = 500;
	params.procedure.windowSizeLatencyCalcUplink = 40;
	params.procedure.windowSizeLatencyCalcDownlink = 41;
	params.procedure.windowSizePacketLossCalcUplink = 42;
	params.procedure.windowSizePacketLossCalcDownlink = 43;


	std::string sdpMessage = Q4SSDP_create(params);
	std::string compare;
	
	compare.append(QOSLEVEL_PATTERN);
	compare.append("1/3\n");
	compare.append(ALERTINGMODE_PATTERN);
	compare.append(Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN);
	compare.append("\n");
	compare.append(ALERTPAUSE_PATTERN);
	compare.append("25000\n");
	compare.append(RECOVERYPAUSE_PATTERN);
	compare.append("40000\n");
	compare.append(APPLATENCY_PATTERN);
	compare.append("6\n");
	compare.append(APPJITTER_PATTERN);
	compare.append("5/6\n");
	compare.append(APPBANDWIDTH_PATTERN);
	compare.append("4000/3000\n");
	compare.append(APPPACKETLOSS_PATTERN);
	compare.append("0.3/0.3\n");
	compare.append(PROCEDURE_PATTERN);
	compare.append("10/12,13/14,500,40/41,42/43");
	compare.append(PROCEDURE_CLOSE_PATTERN);
	compare.append("\n");
	EXPECT_EQ(compare, sdpMessage);

	Q4SSDPParams paramsReaded;
	bool ok = Q4SSDP_parse(sdpMessage, paramsReaded);
	EXPECT_EQ(ok, true);
	EXPECT_EQ(paramsReaded, params);
}

TEST(Q4SSDP_test_case, testQ4SSDPQ4SSDP_parsePublicServerAddress)
{
	Q4SSDPParams params;
	params.qosLevelUp = 1;
	params.qosLevelDown = 3;
	params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	params.alertPause = 25000;
	params.recoveryPause = 40000;
	params.latency = 6;
	params.jitterUp = 5;
	params.jitterDown = 6;
	params.bandWidthUp = 4000;
	params.bandWidthDown = 3000;
	params.packetLossUp = 0.3f;
	params.packetLossDown = 0.3f;
	params.procedure.negotiationTimeBetweenPingsUplink = 10;
	params.procedure.negotiationTimeBetweenPingsDownlink = 12;
	params.procedure.continuityTimeBetweenPingsUplink = 13;
	params.procedure.continuityTimeBetweenPingsDownlink = 14;
	params.procedure.bandwidthTime = 500;
	params.procedure.windowSizeLatencyCalcUplink = 40;
	params.procedure.windowSizeLatencyCalcDownlink = 41;
	params.procedure.windowSizePacketLossCalcUplink = 42;
	params.procedure.windowSizePacketLossCalcDownlink = 43;
	params.publicServerAddress = "127.0.0.1";

	std::string sdpMessage = Q4SSDP_create(params);
	std::string compare;
	
	compare.append(QOSLEVEL_PATTERN);
	compare.append("1/3\n");
	compare.append(ALERTINGMODE_PATTERN);
	compare.append(Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN);
	compare.append("\n");
	compare.append(ALERTPAUSE_PATTERN);
	compare.append("25000\n");
	compare.append(RECOVERYPAUSE_PATTERN);
	compare.append("40000\n");
	compare.append(APPLATENCY_PATTERN);
	compare.append("6\n");
	compare.append(APPJITTER_PATTERN);
	compare.append("5/6\n");
	compare.append(APPBANDWIDTH_PATTERN);
	compare.append("4000/3000\n");
	compare.append(APPPACKETLOSS_PATTERN);
	compare.append("0.3/0.3\n");
	compare.append(PROCEDURE_PATTERN);
	compare.append("10/12,13/14,500,40/41,42/43");
	compare.append(PROCEDURE_CLOSE_PATTERN);
	compare.append("\n");
	compare.append(PUBLIC_ADDRESS_SERVER_PATTERN);
	compare.append("127.0.0.1\n");
	EXPECT_EQ(compare, sdpMessage);

	Q4SSDPParams paramsReaded;
	bool ok = Q4SSDP_parse(sdpMessage, paramsReaded);
	EXPECT_EQ(ok, true);
	ok = Q4SSDP_parsePublicServerAddress(sdpMessage, paramsReaded);
	EXPECT_EQ(ok, true);
	EXPECT_EQ(paramsReaded, params);
}

TEST(Q4SSDP_test_case, testQ4SSDPQ4SSDP_parsePublicClientAddress)
{
	Q4SSDPParams params;
	params.qosLevelUp = 1;
	params.qosLevelDown = 3;
	params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
	params.alertPause = 25000;
	params.recoveryPause = 40000;
	params.latency = 6;
	params.jitterUp = 5;
	params.jitterDown = 6;
	params.bandWidthUp = 4000;
	params.bandWidthDown = 3000;
	params.packetLossUp = 0.3f;
	params.packetLossDown = 0.3f;
	params.procedure.negotiationTimeBetweenPingsUplink = 10;
	params.procedure.negotiationTimeBetweenPingsDownlink = 12;
	params.procedure.continuityTimeBetweenPingsUplink = 13;
	params.procedure.continuityTimeBetweenPingsDownlink = 14;
	params.procedure.bandwidthTime = 500;
	params.procedure.windowSizeLatencyCalcUplink = 40;
	params.procedure.windowSizeLatencyCalcDownlink = 41;
	params.procedure.windowSizePacketLossCalcUplink = 42;
	params.procedure.windowSizePacketLossCalcDownlink = 43;
	params.publicClientAddress = "127.0.0.2";

	std::string sdpMessage = Q4SSDP_create(params);
	std::string compare;
	
	compare.append(QOSLEVEL_PATTERN);
	compare.append("1/3\n");
	compare.append(ALERTINGMODE_PATTERN);
	compare.append(Q4SSDPALERTINGMODE_Q4SAWARENETWORK_PATTERN);
	compare.append("\n");
	compare.append(ALERTPAUSE_PATTERN);
	compare.append("25000\n");
	compare.append(RECOVERYPAUSE_PATTERN);
	compare.append("40000\n");
	compare.append(APPLATENCY_PATTERN);
	compare.append("6\n");
	compare.append(APPJITTER_PATTERN);
	compare.append("5/6\n");
	compare.append(APPBANDWIDTH_PATTERN);
	compare.append("4000/3000\n");
	compare.append(APPPACKETLOSS_PATTERN);
	compare.append("0.3/0.3\n");
	compare.append(PROCEDURE_PATTERN);
	compare.append("10/12,13/14,500,40/41,42/43");
	compare.append(PROCEDURE_CLOSE_PATTERN);
	compare.append("\n");
	compare.append(PUBLIC_ADDRESS_CLIENT_PATTERN);
	compare.append("127.0.0.2\n");
	EXPECT_EQ(compare, sdpMessage);

	Q4SSDPParams paramsReaded;
	bool ok = Q4SSDP_parse(sdpMessage, paramsReaded);
	EXPECT_EQ(ok, true);
	ok = Q4SSDP_parsePublicClientAddress(sdpMessage, paramsReaded);
	EXPECT_EQ(ok, true);
	EXPECT_EQ(paramsReaded, params);
}