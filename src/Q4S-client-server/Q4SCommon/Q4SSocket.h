#ifndef _Q4SSOCKET_H_
#define _Q4SSOCKET_H_

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#define DEFAULT_TCP_PORT "27015"
#define DEFAULT_UDP_PORT "27016"

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
    bool    sendData( const char* sendBuffer );

    bool    receiveData( char* receiveBuffer, int receiveBufferSize );
    bool    shutDown( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET          mSocket;
    int             mSocketType;

    sockaddr_in     addrReceiver;
    int             addrReceiverLen;

    bool            alreadyReceived;
};

#endif  // _Q4SSOCKET_H_