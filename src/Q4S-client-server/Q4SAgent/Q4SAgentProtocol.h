#ifndef _Q4SAGENTPROTOCOL_H_
#define _Q4SAGENTPROTOCOL_H_

#include "Q4SAgentSocket.h"

#include "..\Q4SCommon\Q4SMessageManager.h"


class Q4SAgentProtocol
{
public:

    // Constructor-Destructor
    Q4SAgentProtocol ();
    ~Q4SAgentProtocol ();

    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
    bool    listen();
    void    end();

private:

    void    clear();

    bool    openConnectionListening();
    void    closeConnectionListening();
    void    closeConnections();

    Q4SAgentSocket              mAgentSocket;
    HANDLE                      marrthrListenHandle;
    HANDLE                      marrthrDataHandle;

    bool                        manageUdpConnection( );
    static DWORD WINAPI         manageUdpConnectionsFn( LPVOID lpData );

    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;
};

#endif  // _Q4SAGENTPROTOCOL_H_