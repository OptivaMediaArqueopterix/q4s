#ifndef _Q4SCLIENTPROTOCOL_H_
#define _Q4SCLIENTPROTOCOL_H_

#include "Q4SClientSocket.h"
#include "Q4SMessageManager.h"
#include <vector>
#include "Q4SStructs.h"

class Q4SClientProtocol
{
public:

    // Constructor-Destructor
    Q4SClientProtocol ();
    ~Q4SClientProtocol ();

    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
    bool    begin();
    bool    ready();
    bool    measure(Q4SMeasurementParams params);
    void    continuity(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss);
    void    bwidth();
    void    cancel();

private:

    void    clear();

    bool    openConnections();
    void    closeConnections();

    bool    measureStage0(float maxLatency, float maxJitter);
    bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps);
    void    calculateLatency(std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showMeasureInfo);
    void    calculateJitter(float &jitter, bool showMeasureInfo);
    bool    checkLatencyAndJitter(float latency, float jitter, float maxLatency, float maxJitter);
    bool    measureStage1(float minBandWith, float maxPacketLoss);

    Q4SClientSocket             mClientSocket;
    HANDLE                      marrthrHandle[ 2 ];
    bool                        manageTcpReceivedData( );
    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageTcpReceivedDataFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;
};

#endif  // _Q4SCLIENTPROTOCOL_H_