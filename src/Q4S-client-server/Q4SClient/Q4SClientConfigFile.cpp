#include "Q4SClientConfigFile.h"

Q4SClientConfigFile q4SClientConfigFile("Config.txt");

Q4SClientConfigFile::Q4SClientConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    serverIP = configFile.getValueOfKey<std::string>("SERVER_IP", "127.0.0.1");
    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    timeBetweenPings = configFile.getValueOfKey<double>("TIME_BETWEEN_PINGS", 200   );

    //bool exists = cfg.keyExists("SERVER_IP");
}