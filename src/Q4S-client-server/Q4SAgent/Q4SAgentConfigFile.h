#ifndef _Q4SAGENTCONFIGFILE_H_
#define _Q4SAGENTCONFIGFILE_H_

#include "..\Q4SCommon\ConfigFile.h"

class Q4SAgentConfigFile
{
public:

    Q4SAgentConfigFile(const std::string &fName);

    std::string ganyConnectorIp;
    std::string ganyConnectorPort;
    std::string listenUDPPort;
    unsigned long timeEndApp;
    unsigned long initialBitRate;
    unsigned long bitRateReduceCuantity;
    bool demoMode;
};

extern Q4SAgentConfigFile q4SAgentConfigFile;

#endif //_Q4SAGENTCONFIGFILE_H_