#ifndef _Q4SSERVERSOCKET_H_
#define _Q4SSERVERSOCKET_H_

#include "Q4SSocket.h"

#include <stdio.h>
#include <list>

class Q4SServerSocket
{
public:

    // Constructor-Destructor
    Q4SServerSocket( );
    ~Q4SServerSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    bool    startTcpListening( );
    bool    waitForTcpConnection( );
    bool    waitForUdpConnections( );
    bool    stopWaiting( );
    bool    closeConnection( int socketType );
    bool    sendTcpData( const char* sendBuffer );
    bool    receiveTcpData( int connId, char* receiveBuffer, int receiveBufferSize );
    bool    sendUdpData( const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize );

private:

    struct Q4SConnectionInfo 
    {
        int             id;
        int             udpId;
        Q4SSocket       mq4sTcpSocket;
        sockaddr_in     peerAddrInfo;
    };

    void    clear( );

    bool    initializeSockets( );
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SConnectionInfo* connectionInfo );
    bool    closeListenSocket( );
    bool    createUdpSocket( );
    bool    bindUdpSocket( );

    SOCKET              mListenSocket;
    SOCKET              mUdpSocket;
    struct addrinfo*    mpAddrInfoResultTcp; 
    struct addrinfo*    mpAddrInfoResultUdp; 

    Q4SSocket           mq4sTcpSocket;
    Q4SSocket           mq4sUdpSocket;

    std::list< Q4SConnectionInfo* >         listConnectionInfo;
};

#endif  // _Q4SSERVERSOCKET_H_