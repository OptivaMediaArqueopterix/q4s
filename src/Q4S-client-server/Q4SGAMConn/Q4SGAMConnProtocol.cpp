#include "Q4SGAMConnProtocol.h"

#include "Q4SGAMConnConfigFile.h"

#include "..\Q4SCommon\ETime.h"
#include "..\Q4SCommon\Q4SMathUtils.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#define     DEFAULT_CONN_ID     1

Q4SGAMConnProtocol::Q4SGAMConnProtocol ()
{
    clear();
}

Q4SGAMConnProtocol::~Q4SGAMConnProtocol ()
{
    done();
}

bool Q4SGAMConnProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    if (ok)
    {
        ok &= mGAMConnSocket.init();
    }

    if (ok)
    {
        ok &= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnectionListening();
    }
    
    return ok;
}

void Q4SGAMConnProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SGAMConnProtocol::clear()
{
}

bool Q4SGAMConnProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SGAMConnProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        WaitForSingleObject( marrthrListenHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SGAMConnProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mGAMConnSocket.closeConnection( SOCK_DGRAM );
        WaitForSingleObject( marrthrDataHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


// State managing functions.

bool Q4SGAMConnProtocol::listen()
{
    printf("METHOD: listen at port %s\n", q4SGAMConnConfigFile.listenUDPPort.c_str());
    std::string message;

    bool ok = true;
    
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message );
    }

    return ok;
}


void Q4SGAMConnProtocol::end()
{
    closeConnections();
}

// Received data managing functions.

DWORD WINAPI Q4SGAMConnProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SGAMConnProtocol* q4sCP = ( Q4SGAMConnProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SGAMConnProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];

    mGAMConnSocket.startUdpListening( );

    while ( ok )
    {
        ok &= mGAMConnSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ));

        if( ok )
        {
            // TODO: Leer el buffer para ver que alerta es y actuar en consecuencia
            printf( "Received Udp: <%s>\n", udpBuffer );
        }
    }

    return ok;
}