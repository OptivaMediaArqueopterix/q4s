#include "Q4SServerConfigFile.h"

Q4SServerConfigFile q4SServerConfigFile("Q4SServerConfig.txt");

Q4SServerConfigFile::Q4SServerConfigFile(const std::string &fName)
{
    ConfigFile configFile(fName);

    defaultTCPPort = configFile.getValueOfKey<std::string>("DEFAULT_TCP_PORT", "27015");
    defaultUDPPort = configFile.getValueOfKey<std::string>("DEFAULT_UDP_PORT", "27016");
    agentIP = configFile.getValueOfKey<std::string>("AGENT_IP", "127.0.0.1");
    agentPort = configFile.getValueOfKey<std::string>("AGENT_PORT", "27017");
    isReactive = configFile.keyExists("IS_REACTIVE");
    alertPause = configFile.getValueOfKey<unsigned long>("ALERT_PAUSE", 2000);
	recoveryPause = configFile.getValueOfKey<unsigned long>("RECOVERY_PAUSE", 4000);
    latency = configFile.getValueOfKey<unsigned long>("LATENCY", 3);
    jitterUp = configFile.getValueOfKey<unsigned long>("JITTER_UP", 200);
    jitterDown = configFile.getValueOfKey<unsigned long>("JITTER_DOWN", 200);
	bandwidthUp = configFile.getValueOfKey<unsigned long>("BANDWIDTH_UP", 400);
	bandwidthDown = configFile.getValueOfKey<unsigned long>("BANDWIDTH_DOWN", 400);
	packetLossUp = configFile.getValueOfKey<float>("PACKETLOSS_UP", 1.00f);
	packetLossDown = configFile.getValueOfKey<float>("PACKETLOSS_UP", 1.00f);
    timeBetweenPingsNegotiationUp = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_NEGOTITATION_UP", 20);
    timeBetweenPingsNegotiationDown = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_NEGOTITATION_DOWN", 20);
    timeBetweenPingsContinuityUp = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_CONTINUITY_UP", 20);
    timeBetweenPingsContinuityDown = configFile.getValueOfKey<unsigned long>("TIME_BETWEEN_PINGS_CONTINUITY_DOWN", 20);
	bandwidthTime= configFile.getValueOfKey<unsigned long>("BANDWIDTH_TIME", 2000);
    numberOfPingsUp = configFile.getValueOfKey<unsigned long>("NUMBER_OF_PINGS_UP", 20);
    numberOfPingsDown = configFile.getValueOfKey<unsigned long>("NUMBER_OF_PINGS_DOWN", 20);
    numberOfBwidthsUp = configFile.getValueOfKey<unsigned long>("NUMBER_OF_BWIDTHS_UP", 20);
    numberOfBwidthsDown = configFile.getValueOfKey<unsigned long>("NUMBER_OF_BWIDTHS_DOWN", 20);
    timeEndApp = configFile.getValueOfKey<unsigned long>("TIME_END_APP", 10000);
    showMeasureInfo = configFile.keyExists("SHOW_MEASURE_INFO");
    showReceivedPingInfo = configFile.keyExists("SHOW_RECEIVED_PING_INFO");
    showSocketReceivedInfo = configFile.keyExists("SHOW_SOCKET_RECEIVED_INFO");
}