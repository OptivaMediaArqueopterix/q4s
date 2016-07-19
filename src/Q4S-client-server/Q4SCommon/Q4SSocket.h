#ifndef _Q4SSOCKET_H_
#define _Q4SSOCKET_H_

#include "Config.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>


class Q4SSocket
{
public:

    // Constructor-Destructor
    Q4SSocket( );
    ~Q4SSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    void    setSocket( SOCKET socket, int socketType );
    bool    sendData( const char* sendBuffer, sockaddr_in* pAddrInfo = NULL );

    bool    receiveData( char* receiveBuffer, int receiveBufferSize );
    bool    shutDown( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET          mSocket;
    int             mSocketType;

    sockaddr_in     mPeerAddrInfo;
    int             mPeerAddrInfoLen;

    bool            mAlreadyReceived;
};

#endif  // _Q4SSOCKET_H_