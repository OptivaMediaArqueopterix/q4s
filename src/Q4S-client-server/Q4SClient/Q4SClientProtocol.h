#ifndef _Q4SCLIENTPROTOCOL_H_
#define _Q4SCLIENTPROTOCOL_H_

#include "Q4SClientSocket.h"
#include "Q4SMessageManager.h"

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
    bool    measureStage0(float maxLatency, float maxJitter);
    void    bwidth();
    void    cancel();
    void    alert();
    void    end();

private:

    void    clear();

    bool    openConnections();
    void    closeConnections();

    Q4SClientSocket             mClientSocket;
    HANDLE                      marrthrHandle[ 2 ];
    bool                        manageTcpReceivedData( );
    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageTcpReceivedDataFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;

    bool                        isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp);
};

#endif  // _Q4SCLIENTPROTOCOL_H_