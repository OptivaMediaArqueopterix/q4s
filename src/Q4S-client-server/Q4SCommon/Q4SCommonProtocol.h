#ifndef _Q4SCOMMONPROTOCOL_H_
#define _Q4SCOMMONPROTOCOL_H_

#include "Q4SStructs.h"
//#include "Q4SMessageManager.h"

class Q4SCommonProtocol
{
public:


protected:

    //bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps);
    //void    calculateLatency(std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showMeasureInfo);
    //void    calculateJitter(Q4SMessageManager &mReceivedMessages, float &jitter, bool showResult, bool showMeasureInfo, float timeBetweenPings);
    bool    checkStage0(Q4SMeasurementStage0Limits limits, Q4SMeasurementResult &results);
    //bool    measureStage1(Q4SMeasurementStage1Limits limits, Q4SMeasurementResult &results);
};

#endif  // _Q4SCOMMONPROTOCOL_H_