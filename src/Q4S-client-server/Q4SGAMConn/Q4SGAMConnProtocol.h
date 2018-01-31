#ifndef _Q4SGAMCONNPROTOCOL_H_
#define _Q4SGAMCONNPROTOCOL_H_

#include "Q4SGAMConnSocket.h"

#include "..\Q4SCommon\Q4SMessageManager.h"


class Q4SGAMConnProtocol
{
public:

    // Constructor-Destructor
    Q4SGAMConnProtocol ();
    ~Q4SGAMConnProtocol ();

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

    Q4SGAMConnSocket            mGAMConnSocket;
    HANDLE                      marrthrListenHandle;
    HANDLE                      marrthrDataHandle;

    bool                        manageUdpConnection( );
    static DWORD WINAPI         manageUdpConnectionsFn( LPVOID lpData );

    bool                        manageUdpReceivedData( );
    static DWORD WINAPI         manageUdpReceivedDataFn( LPVOID lpData );

    Q4SMessageManager           mReceivedMessages;
};

#endif  // _Q4SGAMCONNPROTOCOL_H_