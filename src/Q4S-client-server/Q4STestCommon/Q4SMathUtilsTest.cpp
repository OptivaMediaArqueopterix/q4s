#include "gtest/gtest.h"

#include "..\Q4SCommon\Q4SMathUtils.h"

TEST(Q4SMathUtils_test_case, testMedian)
{
    std::vector<float> arrPingLatencies;
    float latency;

    arrPingLatencies.push_back( 2 );

    // Latency calculation
    latency = EMathUtils_median( arrPingLatencies );

    EXPECT_EQ(latency, 2);
}