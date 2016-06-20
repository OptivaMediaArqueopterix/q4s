#include "Q4SClientProtocol.h"

#include "ETime.h"

#include <stdio.h>

Q4SClientProtocol::Q4SClientProtocol ()
{
    clear();
}

Q4SClientProtocol::~Q4SClientProtocol ()
{
    done();
}

bool Q4SClientProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    mReceivedMessages.init( );
    ok &= openConnections();

    return ok;
}

void Q4SClientProtocol::done()
{
    //closeConnections();
    mReceivedMessages.done( );
}

bool Q4SClientProtocol::openConnections()
{
    bool ok = true;

    //open connections
    if( ok )
    {
        ok &= mClientSocket.openConnection( SOCK_STREAM );
        ok &= mClientSocket.openConnection( SOCK_DGRAM );
    }

    // launch threads
    if( ok )
    {
        //char buffer[ 65536 ];
        //ok &= mClientSocket.receiveTcpData( buffer, sizeof( buffer ) );
        //printf( "Received: <%s>\n", buffer );
        //ok &= mClientSocket.receiveUdpData( buffer, sizeof( buffer ) );
        //printf( "Received: <%s>\n", buffer );

        marrthrHandle[ 0 ] = CreateThread( 0, 0, manageUdpReceivedDataFn, ( LPVOID )this, 0, 0 );
        marrthrHandle[ 1 ] = CreateThread( 0, 0, manageTcpReceivedDataFn, ( LPVOID )this, 0, 0 );
    }

    return ok;
}

void Q4SClientProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        WaitForMultipleObjects( 2, marrthrHandle, true, INFINITE );
        ok &= mClientSocket.closeConnection( SOCK_STREAM );
        ok &= mClientSocket.closeConnection( SOCK_DGRAM );
    }

    if (!ok)
    {
        //TODO: launch error
    }
}

bool Q4SClientProtocol::begin()
{
    printf("METHOD: begin\n");

    bool ok = true;
    
    if( ok )
    {
        ok &= mClientSocket.sendTcpData( "BEGIN" );
    }

    if ( ok ) 
    {
        std::string message;
        mReceivedMessages.readFirst( message );
    }

    return ok;
}

bool Q4SClientProtocol::ready()
{
    printf("METHOD: ready\n");

    bool ok = true;
    
    if( ok )
    {
        ok &= mClientSocket.sendTcpData( "READY" );
    }

    if ( ok ) 
    {
        std::string message;
        mReceivedMessages.readFirst( message );
    }

    return ok;
}

bool Q4SClientProtocol::ping()
{
    printf("METHOD: ping\n");

    bool    ok = true;
    int     j = 0,
            jmax = 20;
    char    buffer[ 256 ];
    if( ok )
    {
        for( j = 0; j < jmax; j++ )
        {
            unsigned long timestamp = ETime_getTime( );
            printf( "Ping %d at %d\n", j, timestamp );
            sprintf_s( buffer, "PING %d %ld", j, timestamp );
            ok &= mClientSocket.sendUdpData( buffer );
            Sleep( 200 );
        }
    }

    return ok;
}

void Q4SClientProtocol::bwidth()
{
    printf("METHOD: bwidth\n");
}

void Q4SClientProtocol::cancel()
{
    printf("METHOD: cancel\n");
}

void Q4SClientProtocol::alert()
{
    printf("METHOD: alert\n");
}


void Q4SClientProtocol::clear()
{
}

DWORD WINAPI Q4SClientProtocol::manageTcpReceivedDataFn( LPVOID lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    return q4sCP->manageTcpReceivedData( );
}

DWORD WINAPI Q4SClientProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SClientProtocol::manageTcpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    while (ok ) 
    {
        ok &= mClientSocket.receiveTcpData( buffer, sizeof( buffer ) );
        std::string message = buffer;
        mReceivedMessages.addMessage ( message );
        printf( "Received Tcp: <%s>\n", buffer );
    }


    return ok;
}

bool Q4SClientProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    ok &= mClientSocket.receiveUdpData( buffer, sizeof( buffer ) );
    std::string message = buffer;
    mReceivedMessages.addMessage ( message );
    printf( "Received Udp: <%s>\n", buffer );

    return ok;
}
