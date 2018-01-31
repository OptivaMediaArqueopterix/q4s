#ifndef _Q4SCLIENTPROTOCOL_H_
#define _Q4SCLIENTPROTOCOL_H_

#include "Q4SClientSocket.h"

#include "..\Q4SCommon\Q4SCommonProtocol.h"
#include "..\Q4SCommon\Q4SMessageManager.h"
#include "..\Q4SCommon\Q4SStructs.h"
#include "..\Q4SCommon\Q4SSDP.h"

#include <vector>

class Q4SClientProtocol: Q4SCommonProtocol
{
public:

    // Constructor-Destructor
    Q4SClientProtocol ();
    ~Q4SClientProtocol ();

    // Init-Done
    bool    init(unsigned long times, unsigned long milisecondsBetweenTimes);
    void    done();

    // Q4S Methods
    bool    ready(unsigned long stage);

	// Q4S Phases
    bool    handshake(Q4SSDPParams &params);
	bool	negotiation(Q4SSDPParams params, Q4SMeasurementResult &results);
    void    continuity(Q4SSDPParams params);

private:

    void    clear();

    bool    openConnections();
    void    closeConnections();
	bool	tryOpenConnectionsTimes(unsigned long times, unsigned long milisecondsBetweenTimes);

    void    bwidth();
    void    cancel();

	bool    measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults,unsigned long pingsToSend);
    bool	measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend);
	bool    sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings);
    bool    measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults);
	bool	interchangeMeasurementProcedure(Q4SMeasurementValues &downMeasurements, Q4SMeasurementResult results);

    Q4SClientSocket             mClientSocket;
    HANDLE                      marrthrHandle[ 2 ];
    bool                        manageTcpReceivedData( );
    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageTcpReceivedDataFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;
	Q4SMeasurementResult		mResults;
};

#endif  // _Q4SCLIENTPROTOCOL_H_