#ifndef _Q4SSERVERSOCKET_H_
#define _Q4SSERVERSOCKET_H_

#include "..\Q4SCommon\Q4SSocket.h"

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

    // TCP
    bool    startTcpListening( );
    bool    waitForTcpConnection( int connectionId );
    bool    stopWaiting( );
    bool    sendTcpData( int connectionId, const char* sendBuffer );
    bool    receiveTcpData( int connectionId, char* receiveBuffer, int receiveBufferSize );

    // UDP
    bool    startUdpListening( );
    bool    sendUdpData( int connectionId, const char* sendBuffer );
    bool    receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId );

    // Alert
    bool    startAlertSender( );
    bool    sendAlertData( const char* sendBuffer );

    // TCP/UDP
    bool    closeConnection( int socketType );

private:

    struct Q4SConnectionInfo 
    {
        int             id;
        Q4SSocket       q4sTcpSocket;
        sockaddr_in     peerTcpAddrInfo;
        sockaddr_in     peerUdpAddrInfo;
    };

    void    clear( );

    // TCP/UDP
    bool    initializeSockets( );

    // TCP
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SConnectionInfo* connectionInfo );
    bool    closeListenSocket( );

    // TCP AUX
    bool    getTcpSocket( int connectionId, Q4SSocket*& pQ4SSocket );
    bool    getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo );
    bool    getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo );

    //UDP
    bool    createUdpSocket( );
    bool    bindUdpSocket( );
    
    // Alert
    bool    createAlertUdpSocket( );

    // TCP
    SOCKET                          mListenSocket;
    struct addrinfo*                mpAddrInfoResultTcp; 
    std::list<Q4SConnectionInfo*>   listConnectionInfo;

    // UDP
    SOCKET              mUdpSocket;
    struct addrinfo*    mpAddrInfoResultUdp; 
    Q4SSocket           mq4sUdpSocket;

    // Alert
    SOCKET              mAlertSocket;
    struct addrinfo*    mpAddrInfoResultAlert; 
    Q4SSocket           mq4sAlertSocket;
};

#endif  // _Q4SSERVERSOCKET_H_