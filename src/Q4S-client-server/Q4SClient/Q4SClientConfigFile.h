#ifndef _Q4SCLIENTCONFIGFILE_H_
#define _Q4SCLIENTCONFIGFILE_H_

#include "ConfigFile.h"

class Q4SClientConfigFile
{
public:

    Q4SClientConfigFile(const std::string &fName);

    std::string serverIP;
    std::string defaultTCPPort;
    std::string defaultUDPPort;
    double timeBetweenPings;
    double timeStartCalc;
    double timeEndApp;
    float maxLatency;
    float maxJitter;
    bool showMeasureInfo;
    bool showReceivedPingInfo;
    bool showSocketReceivedInfo;
};

extern Q4SClientConfigFile q4SClientConfigFile;

#endif //_Q4SCLIENTCONFIGFILE_H_