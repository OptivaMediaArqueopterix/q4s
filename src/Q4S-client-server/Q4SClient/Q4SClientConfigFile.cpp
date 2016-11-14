#include "Q4SClientConfigFile.h"

Q4SClientConfigFile q4SClientConfigFile("Q4SClientConfig.txt");

Q4SClientConfigFile::Q4SClientConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    serverIP = configFile.getValueOfKey<std::string>("SERVER_IP", "127.0.0.1");
    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    timeBetweenPings = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS", 200   );
    timeStartCalc = configFile.getValueOfKey<unsigned long>("TIME_START_CALC", 2000);
    timeEndApp = configFile.getValueOfKey<unsigned long>("TIME_END_APP", 10000);
    maxLatency = configFile.getValueOfKey<float>("MAX_LATENCY", 2000);
    maxJitter = configFile.getValueOfKey<float>("MAX_JITTER", 200);
    showMeasureInfo = configFile.keyExists("SHOW_MEASURE_INFO");
    showReceivedPingInfo = configFile.keyExists("SHOW_RECEIVED_PING_INFO");
    showSocketReceivedInfo = configFile.keyExists("SHOW_SOCKET_RECEIVED_INFO");
}