#include "Q4SServerConfigFile.h"

Q4SServerConfigFile q4SServerConfigFile("Q4SServerConfig.txt");

Q4SServerConfigFile::Q4SServerConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    agentIP = configFile.getValueOfKey<std::string>("AGENT_IP", "127.0.0.1");
    agentPort = configFile.getValueOfKey<std::string>("AGENT_PORT", "27017");
    timeBetweenPings = configFile.getValueOfKey<double>("TIME_BETWEEN_PINGS", 200   );
    timeStartCalc = configFile.getValueOfKey<double>("TIME_START_CALC", 2000);
    timeEndApp = configFile.getValueOfKey<double>("TIME_END_APP", 10000);
    maxLatency = configFile.getValueOfKey<float>("MAX_LATENCY", 2000);
    maxJitter = configFile.getValueOfKey<float>("MAX_JITTER", 200);
    showMeasureInfo = configFile.keyExists("SHOW_MEASURE_INFO");
    showReceivedPingInfo = configFile.keyExists("SHOW_RECEIVED_PING_INFO");
    showSocketReceivedInfo = configFile.keyExists("SHOW_SOCKET_RECEIVED_INFO");
}