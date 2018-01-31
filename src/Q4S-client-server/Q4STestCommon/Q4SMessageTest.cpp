#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SMessage.h"
#include "..\Q4SCommon\Q4SMessageTools.h"
#include "..\Q4SCommon\Q4SStructs.h"

TEST(Q4SMessage_test_case, testQ4SMessageBegin)
{
	Q4SMessage message;
    bool ok = message.initRequest(Q4SMTYPE_BEGIN, "myIP", "12345");

    EXPECT_EQ(ok, true);

    EXPECT_EQ("BEGIN q4s://myIP:12345  Q4S/1.0\n\n", message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageReady0)
{
	Q4SMessage message;
    bool ok = message.initRequest(Q4SMTYPE_READY, "myIP", "12345", false, 0, false, 0, true, 0);

    EXPECT_EQ(ok, true);

    EXPECT_EQ("READY q4s://myIP:12345  Q4S/1.0\nStage:0\n\n", message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageReady1)
{
	Q4SMessage message;
    bool ok = message.initRequest(Q4SMTYPE_READY, "myIP", "12345", false, 0, false, 0, true, 1);

    EXPECT_EQ(ok, true);

    EXPECT_EQ("READY q4s://myIP:12345  Q4S/1.0\nStage:1\n\n", message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessagePing)
{
	Q4SMessage message;
    bool ok = message.initRequest(Q4SMTYPE_PING, "myIp", "12345", true, 1, true, 54654654);

    EXPECT_EQ(ok, true);

	EXPECT_EQ("PING q4s://myIp:12345  Q4S/1.0\nSequence-Number:1\nTimestamp:54654654\n\n", message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageBWith)
{
	Q4SMessage message;
	bool ok = message.initRequest(Q4SMTYPE_BWIDTH, "myIp", "12345", true, 0, true, 54654654);

    EXPECT_EQ(ok, true);

	std::string compare = "BWIDTH q4s://myIp:12345  Q4S/1.0\nSequence-Number:0\nTimestamp:54654654\n\n";
	Q4SMessageTools_fillBodyToASize(compare, 1024);

	EXPECT_EQ(compare, message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessage200OKResponse)
{
	Q4SMessage message;
	bool ok = message.initResponse(Q4SRESPONSECODE_200, "OK");

    EXPECT_EQ(ok, true);

	std::string compare = "Q4S/1.0 200 OK\n\n";

	EXPECT_EQ(compare, message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessage200OKResponseWithPingNumber)
{
	Q4SMessage message;
	bool ok = message.initResponse(Q4SRESPONSECODE_200, "OK 3");

    EXPECT_EQ(ok, true);

	std::string compare = "Q4S/1.0 200 OK 3\n\n";

	EXPECT_EQ(compare, message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageInitBWith)
{
	Q4SMessage message;
	bool ok = message.initBWidth("myIp", "12345", 0);

    EXPECT_EQ(ok, true);

	std::string compare = "BWIDTH q4s://myIp:12345  Q4S/1.0\nSequence-Number:0\n\n";
	Q4SMessageTools_fillBodyToASize(compare, 1024);

	EXPECT_EQ(compare, message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageInitPing)
{
	Q4SMessage message;
    bool ok = message.initPing("myIp", "12345", 1, 54654654);

    EXPECT_EQ(ok, true);

	EXPECT_EQ("PING q4s://myIp:12345  Q4S/1.0\nSequence-Number:1\nTimestamp:54654654\n\n", message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMessageInitPingWithResults)
{
	Q4SMeasurementValues results;
	results.latency = 5;
	results.jitter = 4;
	results.bandwidth = 40;
	results.packetLoss = 0.3f;

	Q4SMessage message;
    bool ok = message.initPing("myIp", "12345", 1, 54654654, results);

    EXPECT_EQ(ok, true);
	
	std::string compare;

	compare.append("PING q4s://myIp:12345  Q4S/1.0\nSequence-Number:1\nTimestamp:54654654\n");
	compare.append(MEASUREMENTS_PATTERN);
	compare.append("5");
	compare.append(MEASUREMENTS_JITTER_PATTERN);
	compare.append("4");
	compare.append(MEASUREMENTS_PACKETLOSS_PATTERN);
	compare.append("0.3");
	compare.append(MEASUREMENTS_BANDWIDTH_PATTERN);
	compare.append("40");
	compare.append("\n");
	compare.append("\n");

	EXPECT_EQ(compare, message.getMessage());
}

TEST(Q4SMessage_test_case, testQ4SMeasurementResult_createAndQ4SMeasurementResult_parse)
{
	Q4SMeasurementValues results;
	results.latency = 5;
	results.jitter = 4;
	results.bandwidth = 40;
	results.packetLoss = 0.3f;

	std::string sdpMessage = Q4SMeasurementValues_create(results);

	std::string compare;
	compare.append(MEASUREMENTS_PATTERN);
	compare.append("5");
	compare.append(MEASUREMENTS_JITTER_PATTERN);
	compare.append("4");
	compare.append(MEASUREMENTS_PACKETLOSS_PATTERN);
	compare.append("0.3");
	compare.append(MEASUREMENTS_BANDWIDTH_PATTERN);
	compare.append("40");
	compare.append("\n");
	EXPECT_EQ(compare, sdpMessage);

	Q4SMeasurementValues resultsReaded;
	bool ok = Q4SMeasurementValues_parse(sdpMessage, resultsReaded);
	EXPECT_EQ(ok, true);
	EXPECT_EQ(resultsReaded, results);
}