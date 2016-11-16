#ifndef _Q4SSERVERPROTOCOL_H_
#define _Q4SSERVERPROTOCOL_H_

#include "Q4SServerSocket.h"
#include "Q4SMessageManager.h"
#include <vector>
#include "Q4SStructs.h"

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
    bool    measure(Q4SMeasurementLimits measurementLimits, Q4SMeasurementResult &results);
    void    continuity(Q4SMeasurementLimits limits);
    void    bwidth();
    void    cancel();
    void    alert(std::string alertMessage);
    void    end();

private:

    void    clear();

    bool    openConnectionListening();
    void    closeConnectionListening();
    void    closeConnections();

    bool    measureStage0(Q4SMeasurementStage0Limits limits, Q4SMeasurementResult &results);
    bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps);
    void    calculateLatency(std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showMeasureInfo);
    void    calculateJitter(float &jitter, bool showResult, bool showMeasureInfo);
    bool    checkStage0(Q4SMeasurementStage0Limits limits, Q4SMeasurementResult &results);
    bool    measureStage1(Q4SMeasurementStage1Limits limits, Q4SMeasurementResult &results);

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