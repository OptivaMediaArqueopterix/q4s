#ifndef _Q4SCLIENTSOCKET_H_
#define _Q4SCLIENTSOCKET_H_

#include "..\Q4SCommon\Q4SSocket.h"

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

    bool    openConnection( int socketType );
    bool    closeConnection( int socketType );
    bool    sendTcpData( const char* sendBuffer );
    bool    receiveTcpData( char* receiveBuffer, int receiveBufferSize );
    bool    sendUdpData( const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize );

private:

    bool    initializeSockets( );
    bool    connectToServer( Q4SSocket* q4sSocket, int socketType );

    void    clear( );

    Q4SSocket           mq4sTcpSocket;
    Q4SSocket           mq4sUdpSocket;
};

#endif  // _Q4SCLIENTSOCKET_H_