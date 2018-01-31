#ifndef _Q4SGAMCONNCONFIGFILE_H_
#define _Q4SGAMCONNCONFIGFILE_H_

#include "..\Q4SCommon\ConfigFile.h"

class Q4SGAMConnConfigFile
{
public:

    Q4SGAMConnConfigFile(const std::string &fName);

    std::string listenUDPPort;
    unsigned long timeEndApp;
};

extern Q4SGAMConnConfigFile q4SGAMConnConfigFile;

#endif //_Q4SGAMCONNCONFIGFILE_H_