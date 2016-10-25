#ifndef _Q4SAGENTCONFIGFILE_H_
#define _Q4SAGENTCONFIGFILE_H_

#include "ConfigFile.h"

class Q4SAgentConfigFile
{
public:

    Q4SAgentConfigFile(const std::string &fName);

    std::string ganyConnectorIp;
    std::string ganyConnectorPort;
    std::string listenUDPPort;
    double timeEndApp;
};

extern Q4SAgentConfigFile q4SAgentConfigFile;

#endif //_Q4SAGENTCONFIGFILE_H_