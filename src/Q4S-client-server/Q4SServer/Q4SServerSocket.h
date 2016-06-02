#ifndef _Q4SSERVERSOCKET_H_
#define _Q4SSERVERSOCKET_H_

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

class Q4SServerSocket
{
public:

    // Constructor-Destructor
    Q4SServerSocket( );
    ~Q4SServerSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    initializeSockets( );
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( );
    bool    receiveData( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET              mListenSocket;
    SOCKET              mClientSocket;
    struct addrinfo*    mpAddrInfoResult; 

};

#endif  // _Q4SSERVERSOCKET_H_