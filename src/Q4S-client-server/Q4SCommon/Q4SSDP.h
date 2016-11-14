#ifndef _Q4SSDP_H_
#define _Q4SSDP_H_

#include <string>

enum Q4SSDPAlertingMode
{ Q4SSDPALERTINGMODE_INVALID
, Q4SSDPALERTINGMODE_Q4SAWARENETWORK
, Q4SSDPALERTINGMODE_POLICYSERVER
};

struct Q4SSDPProcedure
{
    unsigned long negotiationTimeBetweenPingsUplink;
    unsigned long negotiationTimeBetweenPingsDownlink;
    unsigned long continuityTimeBetweenPingsUplink;
    unsigned long continuityTimeBetweenPingsDownlink;
    unsigned long bandWithTime;
    unsigned long windowSizeLatencyCalcUplink;
    unsigned long windowSizeLatencyCalcDownlink;
    unsigned long windowSizePacketLossCalcUplink;
    unsigned long windowSizePacketLossCalcDownlink;
};

struct Q4SSDPMeasurement
{
    Q4SSDPProcedure procedure;
    float latency;
    float jitter;
    float bandWith;
    float packetLoss;
};

struct Q4SSDPParams
{
    int qosLevelup; 
    int qosLeveldown; 
    Q4SSDPAlertingMode q4SSDPAlertingMode; 
    unsigned long networkAlertPause;
    unsigned long appAlertPause;
    std::string clientAddress;
    std::string serverAddress;
    unsigned long networkLatency;
    unsigned long networkJitter;
    unsigned long networkBandWidth;
    unsigned long networkPacketLoss;
    unsigned long applicationLatency;
    unsigned long applicationJitter;
    unsigned long applicationBandWidth;
    unsigned long applicationPacketLoss;
};

class Q4SSDP
{
    public:

    // Constructor-Destructor
    Q4SSDP( );
    ~Q4SSDP( );

    // Init-Done
    bool    init( Q4SSDPParams q4SSDPParams );
    void    done( );

    // Get Message
    std::string getMessage();

private:

    void    clear( );

    std::string mMessage;
};

#endif  // _Q4SSDP_H_
