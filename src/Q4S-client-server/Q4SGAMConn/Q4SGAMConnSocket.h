#ifndef _Q4SGAMCONNSOCKET_H_
#define _Q4SGAMCONNSOCKET_H_

#include "..\Q4SCommon\Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SGAMConnSocket
{
public:

    // Constructor-Destructor
    Q4SGAMConnSocket( );
    ~Q4SGAMConnSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    startUdpListening( );
    //bool    sendUdpData( int connectionId, const char* sendBuffer );
    bool    receiveUdpData (char* receiveBuffer, int receiveBufferSize);
    bool    closeConnection( int socketType );

private:

    void    clear( );

    bool    initializeSockets( );
    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    SOCKET              mUdpSocket;
    struct addrinfo*    mpAddrInfoResultUdp; 
    Q4SSocket           mq4sUdpSocket;

};

#endif  // _Q4SGAMCONNSOCKET_H_