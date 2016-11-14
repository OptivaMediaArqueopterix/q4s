#include "Q4SGAMConnConfigFile.h"

Q4SGAMConnConfigFile q4SGAMConnConfigFile("Q4SGAMConnConfig.txt");

Q4SGAMConnConfigFile::Q4SGAMConnConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    listenUDPPort = configFile.getValueOfKey<std::string>("LISTEN_UDP_PORT", "27017");
    timeEndApp = configFile.getValueOfKey<unsigned long>("TIME_END_APP", 10000);
}