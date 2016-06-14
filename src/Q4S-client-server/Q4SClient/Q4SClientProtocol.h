#ifndef _Q4SCLIENTPROTOCOL_H_
#define _Q4SCLIENTPROTOCOL_H_

#include "Q4SClientSocket.h"

class Q4SClientProtocol
{
public:

    // Constructor-Destructor
    Q4SClientProtocol ();
    ~Q4SClientProtocol ();

    // Init-Done
    bool    init();
    void    done();

    // Q4S Methods
    void    begin();
    void    ready();
    void    ping();
    void    bwidth();
    void    cancel();
    void    alert();

private:

    void    clear();

    Q4SClientSocket             mClientSocket;
    HANDLE                      marrthrHandle[ 2 ];
    bool                        manageTcpResponses( );
    bool                        manageUdpResponses( );
    static DWORD WINAPI         manageTcpResponsesFn( LPVOID lpData );
    static DWORD WINAPI         manageUdpResponsesFn( LPVOID lpData );
};

#endif  // _Q4SCLIENTPROTOCOL_H_