#include "Q4SCommonProtocol.h"

#include <stdio.h>
#include "Q4SMessageInfo.h"
#include <vector>
//#include "Q4SMathUtils.h"
/*
void Q4SCommonProtocol::calculateJitter(Q4SMessageManager &mReceivedMessages, float &jitter, bool showResult, bool showMeasureInfo, float timeBetweenPings)
{
    int pingIndex = 0;
    int pingMaxCount = 20;
    Q4SMessageInfo messageInfo;
    std::vector<unsigned long> arrReceivedPingTimestamps;
    std::vector<unsigned long> arrPingJitters;
    unsigned long actualPingTimeWithPrevious;

    // Prepare for Jitter calculation
    for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
    {
        if( mReceivedMessages.readPingMessage( pingIndex, messageInfo, true ) == true )
        {
            arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
            if( pingIndex > 0 )
            {
                // Actual time between this ping and previous calculation
                actualPingTimeWithPrevious = ( arrReceivedPingTimestamps[ pingIndex ] - arrReceivedPingTimestamps[ pingIndex - 1 ] );

                // Actual time between this ping and previous store
                arrPingJitters.push_back( actualPingTimeWithPrevious );

                if (showMeasureInfo)
                {
                    printf( "PING %d ET: %.3f\n", pingIndex, actualPingTimeWithPrevious );
                }
            }
        }
    }

    // Jitter calculation
    float meanOfTimeWithPrevious = EMathUtils_mean( arrPingJitters );
    jitter = meanOfTimeWithPrevious - (float)timeBetweenPings;

    if (showMeasureInfo)
    {
        printf( "Time With previous ping mean: %.3f\n", meanOfTimeWithPrevious );
    }
}
*/
bool Q4SCommonProtocol::checkStage0(Q4SMeasurementStage0Limits limits, Q4SMeasurementResult &results)
{
    bool ok = true;

    if ( results.values.latency > limits.maxLatency )
    {
        results.latency = true;
        printf( "Lantecy limits not reached\n");
        ok = false;
    }

    if (( results.values.jitter > limits.maxJitter) || (results.values.jitter < -limits.maxJitter) )
    {
        results.jitter = true;
        printf( "Jitter limits not reached\n");
        ok = false;
    }

    return ok;
}

