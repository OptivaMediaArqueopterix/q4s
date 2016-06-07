#ifndef _Q4SSOCKET_H_
#define _Q4SSOCKET_H_

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

    void    setSocket( SOCKET socket );
    bool    sendData( char* sendBuffer );

    bool    receiveData( char* receiveBuffer, int receiveBufferSize );
    bool    shutDown( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET         mSocket;
};

#endif  // _Q4SCLIENTSOCKET_H_