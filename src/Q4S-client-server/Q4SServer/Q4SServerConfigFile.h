#ifndef _Q4SSERVERCONFIGFILE_H_
#define _Q4SSERVERCONFIGFILE_H_

#include "..\Q4SCommon\ConfigFile.h"

class Q4SServerConfigFile
{
public:

    Q4SServerConfigFile(const std::string &fName);

    std::string defaultTCPPort;
    std::string defaultUDPPort;
    std::string agentIP;
    std::string agentPort;
	bool isReactive;
    unsigned long alertPause;
    unsigned long recoveryPause;
    unsigned long latency;
    unsigned long jitterUp;
    unsigned long jitterDown;
    unsigned long bandwidthUp;
    unsigned long bandwidthDown;
	float packetLossUp;
	float packetLossDown;
    unsigned long timeBetweenPingsNegotiationUp;
    unsigned long timeBetweenPingsNegotiationDown;
    unsigned long timeBetweenPingsContinuityUp;
    unsigned long timeBetweenPingsContinuityDown;
    unsigned long bandwidthTime;
    unsigned long numberOfPingsUp;
    unsigned long numberOfPingsDown;
    unsigned long numberOfBwidthsUp;
    unsigned long numberOfBwidthsDown;
    unsigned long timeEndApp;
    bool showMeasureInfo;
    bool showReceivedPingInfo;
    bool showSocketReceivedInfo;
	bool showPublicIPAddress;
};

extern Q4SServerConfigFile q4SServerConfigFile;

#endif //_Q4SSERVERCONFIGFILE_H_