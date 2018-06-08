#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SStructs.h"

TEST(Q4SStructs_test_case, testQ4SMeasurementResultAsignation)
{
	Q4SMeasurementResult resultsInitial;
	resultsInitial.latencyAlert = true;
	resultsInitial.jitterAlert = true;
	resultsInitial.bandwidthAlert = true;
	resultsInitial.packetLossAlert = true;
	resultsInitial.values.latency = 1.f;
	resultsInitial.values.jitter = 1.f;
	resultsInitial.values.bandwidth = 1.f;
	resultsInitial.values.packetLoss = 1.f;
	Q4SMeasurementResult resultsCopy;

	resultsCopy = resultsInitial;

	EXPECT_EQ(resultsInitial, resultsCopy);
}

TEST(Q4SStructs_test_case, testQ4SMeasurementValuesAsignation)
{
	Q4SMeasurementValues valuesInitial;
	valuesInitial.latency = true;
	valuesInitial.jitter = true;
	valuesInitial.bandwidth = true;
	valuesInitial.packetLoss = true;
	Q4SMeasurementValues valuesCopy;

	valuesCopy = valuesInitial;

	EXPECT_EQ(valuesInitial, valuesCopy);
}