#ifndef _Q4SSERVERSOCKET_H_
#define _Q4SSERVERSOCKET_H_

#include "Q4SSocket.h"

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

    bool    waitForConnections( );
    bool    stopWaiting( );
    bool    closeConnection( );
    bool    sendData( char* sendBuffer );
    bool    receiveData( char* receiveBuffer, int receiveBufferSize );

private:

    void    clear( );

    bool    initializeSockets( );
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SSocket* q4sSocket );
    bool    closeListenSocket( );

    SOCKET              mListenSocket;
    struct addrinfo*    mpAddrInfoResult; 
    Q4SSocket           mq4sSocket;

};

#endif  // _Q4SSERVERSOCKET_H_