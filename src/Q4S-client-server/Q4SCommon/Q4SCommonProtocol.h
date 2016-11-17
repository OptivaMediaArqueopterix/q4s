#ifndef _Q4SCOMMONPROTOCOL_H_
#define _Q4SCOMMONPROTOCOL_H_

#include "Q4SStructs.h"
#include <vector>

class Q4SMessageManager;

class Q4SCommonProtocol
{
    protected:

        void    calculateLatency(Q4SMessageManager &mReceivedMessages, std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showMeasureInfo);
        void    calculateJitter(Q4SMessageManager &mReceivedMessages, float &jitter, unsigned long timeBetweenPings, bool showMeasureInfo);
        bool    checkStage0(Q4SMeasurementStage0Limits limits, Q4SMeasurementResult &results);
};

#endif  // _Q4SCOMMONPROTOCOL_H_