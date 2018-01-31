#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SMessage.h"
#include "..\Q4SCommon\Q4SMessageTools.h"

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_is200OKMessageCorrect)
{
	Q4SMessage message;
	bool ok = message.initResponse(Q4SRESPONSECODE_200, "OK 3");

    EXPECT_EQ(ok, true);

	bool check = Q4SMessageTools_is200OKMessage(message.getMessage());

	EXPECT_EQ(check, true);
}

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_is200OKMessageNotCorrect)
{
	Q4SMessage message;
	bool ok = message.initResponse(Q4SRESPONSECODE_3XX, "OK 3");

    EXPECT_EQ(ok, true);

	bool check = Q4SMessageTools_is200OKMessage(message.getMessage());

	EXPECT_EQ(check, false);
}

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_isPingMessageCorrect)
{
	Q4SMessage message;
	bool ok = message.initRequest(Q4SMTYPE_PING, "host", "1234");

    EXPECT_EQ(ok, true);

	int pingNumber;
	unsigned long timeStamp;
	bool check = Q4SMessageTools_isPingMessage(message.getMessage(), &pingNumber, &timeStamp);

	EXPECT_EQ(check, true);
}

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_isPingMessageNotCorrect)
{
	Q4SMessage message;
	bool ok = message.initRequest(Q4SMTYPE_BEGIN, "host", "1234");

    EXPECT_EQ(ok, true);

	int pingNumber;
	unsigned long timeStamp;
	bool check = Q4SMessageTools_isPingMessage(message.getMessage(), &pingNumber, &timeStamp);

	EXPECT_EQ(check, false);
}

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_isBandWidthMessageCorrect)
{
	Q4SMessage message;
	bool ok = message.initRequest(Q4SMTYPE_BWIDTH, "host", "1234");

    EXPECT_EQ(ok, true);

	int pingNumber;
	bool check = Q4SMessageTools_isBandWidthMessage(message.getMessage(), &pingNumber);

	EXPECT_EQ(check, true);
}

TEST(Q4SMessageTools_test_case, testQ4SMessageTools_isBandWidthMessageNotCorrect)
{
	Q4SMessage message;
	bool ok = message.initRequest(Q4SMTYPE_BEGIN, "host", "1234");

    EXPECT_EQ(ok, true);

	int pingNumber;
	bool check = Q4SMessageTools_isBandWidthMessage(message.getMessage(), &pingNumber);

	EXPECT_EQ(check, false);
}

