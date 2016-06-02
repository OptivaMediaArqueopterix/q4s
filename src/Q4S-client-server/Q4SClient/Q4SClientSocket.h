#ifndef _Q4SCLIENTSOCKET_H_
#define _Q4SCLIENTSOCKET_H_

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

class Q4SClientSocket
{
public:

    // Constructor-Destructor
    Q4SClientSocket( );
    ~Q4SClientSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    initializeSockets( );
    bool    connectToServer( );
    bool    sendData( );
    bool    receiveData( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET              mClientSocket;
    struct addrinfo*    mpAddrInfoResult; 

};

#endif  // _Q4SCLIENTSOCKET_H_