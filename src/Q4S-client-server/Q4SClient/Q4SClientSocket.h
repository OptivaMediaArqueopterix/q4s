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

    bool    initializeSockets( );
    bool    connectToServer( Q4SSocket* q4sSocket );

private:

    void    clear( );

};

#endif  // _Q4SCLIENTSOCKET_H_