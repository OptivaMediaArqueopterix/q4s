#ifndef _Q4SSERVERPROTOCOL_H_
#define _Q4SSERVERPROTOCOL_H_

#include "Q4SServerSocket.h"
#include "Q4SMessageManager.h"
#include <vector>

class Q4SServerProtocol
{
public:

    // Constructor-Destructor
    Q4SServerProtocol ();
    ~Q4SServerProtocol ();

    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
    bool    begin();
    bool    ready();
    bool    measure(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss);
    void    continuity(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss);
    void    bwidth();
    void    cancel();
    void    alert();
    void    end();

private:

    void    clear();

    bool    openConnectionListening();
    void    closeConnectionListening();
    void    closeConnections();

    bool    measureStage0(float maxLatency, float maxJitter);
    bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps);
    void    calculateLatency(std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showMeasureInfo);
    void    calculateJitter(float &jitter, bool showResult, bool showMeasureInfo);
    bool    checkLatencyAndJitter(float latency, float jitter, float maxLatency, float maxJitter);
    bool    measureStage1(float minBandWith, float maxPacketLoss);

    Q4SServerSocket             mServerSocket;
    HANDLE                      marrthrListenHandle[ 2 ];
    HANDLE                      marrthrDataHandle[ 2 ];

    bool                        manageTcpConnection( );
    bool                        manageUdpConnection( );
    static DWORD WINAPI         manageTcpConnectionsFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpConnectionsFn( LPVOID lpData );
    struct ManageTcpConnectionsFnInfo
    {
        Q4SServerProtocol*      pThis;
        int                     connId;
    };

    bool                        manageTcpReceivedData( int connId );
    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageTcpReceivedDataFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;

    unsigned long               lastAlertTimeStamp;
};

#endif  // _Q4SSERVERPROTOCOL_H_