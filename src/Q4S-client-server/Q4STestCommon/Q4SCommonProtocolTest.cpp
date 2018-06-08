#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SCommonProtocol.h"
#include "..\Q4SCommon\Q4SMessage.h"
#include "..\Q4SCommon\Q4SMessageManager.h"

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithoutJitter)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 2);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 2, packetLoss, false);

    EXPECT_EQ(jitter, 0.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithJitter1)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 3);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 2, packetLoss, false);

    EXPECT_EQ(jitter, 1.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityTwoMessagesWithJitter2)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 2, packetLoss, false);

    EXPECT_EQ(jitter, 2.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithoutJitter)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 2);
	ok = q4SMessage.initPing("", "", 2, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 3, packetLoss, false);

    EXPECT_EQ(jitter, 0.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithPacketLoss)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 2, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 3, packetLoss, false);

    EXPECT_EQ(jitter, 2.f);
    EXPECT_EQ(packetLoss, 100.f/3.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityFourMessagesWithPacketLossTwoGaps)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 3, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 6);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 4, packetLoss, false);

    EXPECT_EQ(jitter, 8.f/3.f);
    EXPECT_EQ(packetLoss, 2.f * 100.f/4.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityFourMessagesWithPacketLossTwoSpaces)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 2, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	ok = q4SMessage.initPing("", "", 4, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 8);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 5, packetLoss, false);

    EXPECT_EQ(jitter, 8.f/4.f);
    EXPECT_EQ(packetLoss, 2.f * 100.f/5.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithJitter1)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 3);
	ok = q4SMessage.initPing("", "", 2, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 6);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 3, packetLoss, false);

    EXPECT_EQ(jitter, 1.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculateJitterAndPacketLossContinuityThreeMessagesWithJitter2)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initPing("", "", 0, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 0);
	ok = q4SMessage.initPing("", "", 1, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	ok = q4SMessage.initPing("", "", 2, 0);
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	
	float jitter = 0.f;
	float packetLoss = 0.f;

	q4SCommonProtocol.calculateJitterAndPacketLossContinuity(q4SMessageManager, jitter, 2, 3, packetLoss, false);

    EXPECT_EQ(jitter, 2.f);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyThreeMessages)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initResponse(Q4SRESPONSECODE_200, "OK");
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 6);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 8);

	arrSentPingTimestamps.push_back(0);
	arrSentPingTimestamps.push_back(4);
	arrSentPingTimestamps.push_back(6);

	float latency = 0.f;

	q4SCommonProtocol.calculateLatency(q4SMessageManager, arrSentPingTimestamps, latency, 3, false);

    EXPECT_EQ(latency, 1.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyTwoMessages)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initResponse(Q4SRESPONSECODE_200, "OK");
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 6);

	arrSentPingTimestamps.push_back(0);
	arrSentPingTimestamps.push_back(4);

	float latency = 0.f;

	q4SCommonProtocol.calculateLatency(q4SMessageManager, arrSentPingTimestamps, latency, 2, false);

    EXPECT_EQ(latency, 1.5f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyOneMessage)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initResponse(Q4SRESPONSECODE_200, "OK");
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);

	arrSentPingTimestamps.push_back(0);

	float latency = 0.f;

	q4SCommonProtocol.calculateLatency(q4SMessageManager, arrSentPingTimestamps, latency, 1, false);

    EXPECT_EQ(latency, 2.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCalculateLatencyOneMessageLost)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	std::vector<unsigned long> arrSentPingTimestamps;

	Q4SMessage q4SMessage;
	bool ok = q4SMessage.initResponse(Q4SRESPONSECODE_200, "OK");
    EXPECT_EQ(ok, true);
	q4SMessageManager.addMessage(q4SMessage.getMessage(), 4);

	arrSentPingTimestamps.push_back(0);
	arrSentPingTimestamps.push_back(4);

	float latency = 0.f;

	q4SCommonProtocol.calculateLatency(q4SMessageManager, arrSentPingTimestamps, latency, 2, false);

    EXPECT_EQ(latency, 2.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1OneMessageBegin)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initRequest(Q4SMTYPE_BEGIN, "", "");
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, false);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1OneMessageBWidth)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithoutPacketLoss)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 1);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 0.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithPacketLoss)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 2);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 100.f/3.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1TwoMessageBWidthWithPacketLossTwoGaps)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 3);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 2.f/4.f*100.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1ThreeMessageBWidthWithPacketLoss)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 2);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 3);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 1.f/4.f*100.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1FourMessageBWidthWithPacketLoss)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 2);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 3);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 4);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 1.f/5.f*100.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolcalculatePacketLossStage1ThreeMessageBWidthWithPacketLossTwoSpaces)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMessageManager q4SMessageManager;
	Q4SMessage q4SMessage;
	q4SMessage.initBWidth("", "", 0);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 2);
	q4SMessageManager.addMessage(q4SMessage.getMessage());
	q4SMessage.initBWidth("", "", 4);
	q4SMessageManager.addMessage(q4SMessage.getMessage());

	float packetLoss = 0.f;

	bool ok = q4SCommonProtocol.calculatePacketLossStage1(q4SMessageManager, packetLoss);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(packetLoss, 2.f/5.f * 100.f);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0BothAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.latency = 3;
	results.values.jitter = 3;

	bool ok = q4SCommonProtocol.checkStage0(2, 2, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.latencyAlert, true);
    EXPECT_EQ(results.jitterAlert, true);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0LatencyAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.latency = 3;
	results.values.jitter = 1;

	bool ok = q4SCommonProtocol.checkStage0(2, 2, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.latencyAlert, true);
    EXPECT_EQ(results.jitterAlert, false);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0JitterAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.latency = 1;
	results.values.jitter = 3;

	bool ok = q4SCommonProtocol.checkStage0(2, 2, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.latencyAlert, false);
    EXPECT_EQ(results.jitterAlert, true);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage0NoAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.latency = 1;
	results.values.jitter = 1;

	bool ok = q4SCommonProtocol.checkStage0(2, 2, results);
    EXPECT_EQ(ok, true);
	EXPECT_EQ(results.latencyAlert, false);
    EXPECT_EQ(results.jitterAlert, false);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1BothAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.bandwidth = 1.f;
	results.values.packetLoss = 3.f;

	bool ok = q4SCommonProtocol.checkStage1(2, 2.f, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.bandwidthAlert, true);
    EXPECT_EQ(results.packetLossAlert, true);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1BandwidthAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.bandwidth = 1.f;
	results.values.packetLoss = 1.f;

	bool ok = q4SCommonProtocol.checkStage1(2, 2.f, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.bandwidthAlert, true);
    EXPECT_EQ(results.packetLossAlert, false);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1PacketLossAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.bandwidth = 3.f;
	results.values.packetLoss = 3.f;

	bool ok = q4SCommonProtocol.checkStage1(2, 2.f, results);
    EXPECT_EQ(ok, false);
	EXPECT_EQ(results.bandwidthAlert, false);
    EXPECT_EQ(results.packetLossAlert, true);
}

TEST(Q4SCommonProtocol_test_case, testQ4SCommonProtocolCheckStage1NoAlert)
{
	Q4SCommonProtocol q4SCommonProtocol;
	Q4SMeasurementResult results;
	results.values.bandwidth = 3.f;
	results.values.packetLoss = 1.f;

	bool ok = q4SCommonProtocol.checkStage1(2, 2.f, results);
    EXPECT_EQ(ok, true);
	EXPECT_EQ(results.bandwidthAlert, false);
    EXPECT_EQ(results.packetLossAlert, false);
}
