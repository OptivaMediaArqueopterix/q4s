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
    double timeBetweenPings;
    double timeStartCalc;
    double timeEndApp;
    float maxLatency;
    float maxJitter;
};

extern Q4SServerConfigFile q4SServerConfigFile;

#endif //_Q4SSERVERCONFIGFILE_H_