#include "Q4SClientConfigFile.h"

Q4SClientConfigFile q4SClientConfigFile("Q4SClientConfig.txt");

Q4SClientConfigFile::Q4SClientConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    serverIP = configFile.getValueOfKey<std::string>("SERVER_IP", "127.0.0.1");
    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    timeBetweenPings = configFile.getValueOfKey<double>("TIME_BETWEEN_PINGS", 200   );
    timeStartCalc = configFile.getValueOfKey<double>("TIME_START_CALC", 2000);
    timeEndApp = configFile.getValueOfKey<double>("TIME_END_APP", 10000);
    maxLatency = configFile.getValueOfKey<float>("MAX_LATENCY", 2000);
    maxJitter = configFile.getValueOfKey<float>("MAX_JITTER", 200);

    //bool exists = cfg.keyExists("SERVER_IP");
}