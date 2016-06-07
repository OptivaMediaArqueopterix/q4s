#ifndef _Q4SCLIENTSOCKET_H_
#define _Q4SCLIENTSOCKET_H_

#include "Q4SSocket.h"

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

    bool    openConnection( );
    bool    closeConnection( );
    bool    sendData( char* sendBuffer );
    bool    receiveData( char* receiveBuffer, int receiveBufferSize );

private:

    bool    initializeSockets( );
    bool    connectToServer( Q4SSocket* q4sSocket );

    void    clear( );

    Q4SSocket           mq4sSocket;
};

#endif  // _Q4SCLIENTSOCKET_H_