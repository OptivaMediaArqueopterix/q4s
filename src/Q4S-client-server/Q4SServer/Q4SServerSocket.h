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

    bool    initializeSockets( );
    bool    createListenSocket( );
    bool    bindListenSocket( );
    bool    startListen( );
    bool    acceptClientConnection( Q4SSocket* q4sSocket );

private:

    void    clear( );

    SOCKET              mListenSocket;
    struct addrinfo*    mpAddrInfoResult; 

};

#endif  // _Q4SSERVERSOCKET_H_