#ifndef _Q4SSERVERPROTOCOL_H_
#define _Q4SSERVERPROTOCOL_H_

#include "Q4SServerSocket.h"
#include "Q4SMessageManager.h"

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
    bool    ping();
    void    bwidth();
    void    cancel();
    void    alert();

private:

    void    clear();

    bool    openConnections();
    void    closeConnections();

    Q4SServerSocket             mServerSocket;
    HANDLE                      marrthrListenHandle[ 2 ];
    HANDLE                      marrthrDataHandle[ 2 ];

    bool                        manageTcpConnection( );
    bool                        manageUdpConnection( );
    static DWORD WINAPI         manageTcpConnectionsFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpConnectionsFn( LPVOID lpData );

    bool                        manageTcpReceivedData( );
    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageTcpReceivedDataFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;

    bool                        isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp);
};

#endif  // _Q4SSERVERPROTOCOL_H_