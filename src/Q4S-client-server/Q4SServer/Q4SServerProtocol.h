#ifndef _Q4SSERVERPROTOCOL_H_
#define _Q4SSERVERPROTOCOL_H_

#include "Q4SServerSocket.h"

#include "..\Q4SCommon\Q4SCommonProtocol.h"
#include "..\Q4SCommon\Q4SMessageManager.h"
#include "..\Q4SCommon\Q4SStructs.h"
#include "..\Q4SCommon\Q4SSDP.h"

#include <vector>


class Q4SServerProtocol:Q4SCommonProtocol
{
public:

    // Constructor-Destructor
    Q4SServerProtocol ();
    ~Q4SServerProtocol ();

    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
    bool    ready();
    void    end();
    void    alert(std::string alertMessage);
    void    recovery(std::string alertMessage);

	// Q4S Phases
    bool    handshake(Q4SSDPParams &params);
	bool    negotiation(Q4SSDPParams params, Q4SMeasurementResult &results);
    void    continuity(Q4SSDPParams params);

private:

    void    clear();

    bool    openConnectionListening();
    void    closeConnectionListening();
    void    closeConnections();
	
    void    bwidth();
    void    cancel();

    bool    measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend);
	bool	measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend);
    bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings);
    bool    measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult upResults);
	bool	interchangeMeasurementProcedure(Q4SMeasurementValues &upMeasurements, Q4SMeasurementResult results);

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
    unsigned long               recoveryTimeStamp;

	int							qosLevel;
};

#endif  // _Q4SSERVERPROTOCOL_H_