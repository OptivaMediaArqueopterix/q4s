#ifndef _Q4SSERVERCONFIGFILE_H_
#define _Q4SSERVERCONFIGFILE_H_

#include "ConfigFile.h"

class Q4SServerConfigFile
{
public:

    Q4SServerConfigFile(const std::string &fName);

    std::string defaultTCPPort;
    std::string defaultUDPPort;
    double timeBetweenPings;
//#define TIME_START_CALC     2000
//#define TIME_END_APP        10000

};

extern Q4SServerConfigFile q4SServerConfigFile;

#endif //_Q4SSERVERCONFIGFILE_H_