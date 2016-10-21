#ifndef _Q4SCLIENTCONFIGFILE_H_
#define _Q4SCLIENTCONFIGFILE_H_

#include "ConfigFile.h"

class Q4SClientConfigFile
{
public:

    Q4SClientConfigFile(const std::string &fName);

    std::string serverIP;
    double defaultTCPPort;
    double defaultTUPPort;
    double timeBetweenPings;
//#define TIME_START_CALC     2000
//#define TIME_END_APP        10000

};

extern Q4SClientConfigFile q4SClientConfigFile;

#endif //_Q4SCLIENTCONFIGFILE_H_