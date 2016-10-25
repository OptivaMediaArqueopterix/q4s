#include "Q4SServerConfigFile.h"

Q4SServerConfigFile q4SServerConfigFile("Q4SServerConfig.txt");

Q4SServerConfigFile::Q4SServerConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    timeBetweenPings = configFile.getValueOfKey<double>("TIME_BETWEEN_PINGS", 200   );
    timeStartCalc = configFile.getValueOfKey<double>("TIME_START_CALC", 2000);
    timeEndApp = configFile.getValueOfKey<double>("TIME_END_APP", 10000);

    //bool exists = cfg.keyExists("SERVER_IP");
}