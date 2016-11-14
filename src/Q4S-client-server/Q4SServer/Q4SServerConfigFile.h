#ifndef _Q4SSERVERCONFIGFILE_H_
#define _Q4SSERVERCONFIGFILE_H_

#include "ConfigFile.h"

class Q4SServerConfigFile
{
public:

    Q4SServerConfigFile(const std::string &fName);

    std::string defaultTCPPort;
    std::string defaultUDPPort;
    std::string agentIP;
    std::string agentPort;
    unsigned long timeBetweenPings;
    unsigned long timeBetweenAlerts;
    unsigned long timeStartCalc;
    unsigned long timeEndApp;
    float maxLatency;
    float maxJitter;
    bool showMeasureInfo;
    bool showReceivedPingInfo;
    bool showSocketReceivedInfo;
};

extern Q4SServerConfigFile q4SServerConfigFile;

#endif //_Q4SSERVERCONFIGFILE_H_