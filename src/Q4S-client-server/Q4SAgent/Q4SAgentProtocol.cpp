#include "Q4SAgentProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SAgentConfigFile.h"

#define     DEFAULT_CONN_ID     1

Q4SAgentProtocol::Q4SAgentProtocol ()
{
    clear();
}

Q4SAgentProtocol::~Q4SAgentProtocol ()
{
    done();
}

bool Q4SAgentProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    if (ok)
    {
        ok &= mAgentSocket.init();
    }

    if (ok)
    {
        ok &= mAgentSocket.startActionSender();
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

void Q4SAgentProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SAgentProtocol::clear()
{
}

bool Q4SAgentProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SAgentProtocol::closeConnectionListening()
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

void Q4SAgentProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mAgentSocket.closeConnection( SOCK_DGRAM );
        WaitForSingleObject( marrthrDataHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


// State managing functions.

bool Q4SAgentProtocol::listen()
{
    printf("METHOD: listen at port %s\n", q4SAgentConfigFile.listenUDPPort.c_str());
    std::string message;

    bool ok = true;
    
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message );
    }

    return ok;
}


void Q4SAgentProtocol::end()
{
    closeConnections();
}

// Received data managing functions.

DWORD WINAPI Q4SAgentProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SAgentProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];

    mAgentSocket.startUdpListening( );

    while ( ok )
    {
        ok &= mAgentSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ));

        if( ok )
        {
            // TODO: Leer el buffer para ver que alerta es y actuar en consecuencia
            printf( "Received Udp: <%s>\n", udpBuffer );
            mAgentSocket.sendActionData("ALERT");
        }
    }

    return ok;
}