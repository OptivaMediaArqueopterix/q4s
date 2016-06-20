#include "Q4SServerProtocol.h"

#include <stdio.h>

Q4SServerProtocol::Q4SServerProtocol ()
{
    clear();
}

Q4SServerProtocol::~Q4SServerProtocol ()
{
    done();
}

bool Q4SServerProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    mReceivedMessages.init( );

    ok &= openConnections();

    return ok;
}

void Q4SServerProtocol::done()
{
    //closeConnections();
    mReceivedMessages.done( );
}

bool Q4SServerProtocol::openConnections()
{
    bool ok = true;

    marrthrListenHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpConnectionsFn, ( void* ) this, 0, 0 );
    marrthrListenHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpConnectionsFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SServerProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        WaitForMultipleObjects( 2, marrthrListenHandle, true, INFINITE );
        ok &= mServerSocket.closeConnection( SOCK_STREAM );
        ok &= mServerSocket.closeConnection( SOCK_DGRAM );
    }

    if (!ok)
    {
        //TODO: launch error
    }
}

bool Q4SServerProtocol::begin()
{
    printf("METHOD: begin\n");
    std::string message;

    bool ok = true;
    
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message );
    }

    if( ok )
    {
        ok &= mServerSocket.sendTcpData( "200 OK" );
    }

    return ok;
}

bool Q4SServerProtocol::ready()
{
    printf("METHOD: ready\n");
    std::string message;

    bool ok = true;

    if ( ok ) 
    {
        mReceivedMessages.readFirst( message );
    }

    if( ok )
    {
        ok &= mServerSocket.sendTcpData( "200 OK" );
    }

    Sleep( 20000 );

    return ok;
}

bool Q4SServerProtocol::ping()
{
    printf("METHOD: ping\n");

    return 0;
}

void Q4SServerProtocol::bwidth()
{
    printf("METHOD: bwidth\n");
}

void Q4SServerProtocol::cancel()
{
    printf("METHOD: cancel\n");
}

void Q4SServerProtocol::alert()
{
    printf("METHOD: alert\n");
}


void Q4SServerProtocol::clear()
{
}

DWORD WINAPI Q4SServerProtocol::manageTcpConnectionsFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageTcpConnection( );
}

DWORD WINAPI Q4SServerProtocol::manageUdpConnectionsFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageUdpConnection( );
}

bool Q4SServerProtocol::manageTcpConnection( )
{
    bool                ok = true;

    char                buffer[ 65536 ];

    if( ok )
    {
        ok &= mServerSocket.waitForConnections( SOCK_STREAM );
    }

    if( ok )
    {
        marrthrDataHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpReceivedDataFn, ( void* ) this, 0, 0 );
    }
    //while (ok ) 
    //{
    //    if( ok )
    //    {
    //        ok &= mServerSocket.receiveTcpData( buffer, sizeof( buffer ) );
    //        std::string message = buffer;
    //        mReceivedMessages.addMessage ( message );
    //        printf( "Received: <%s>\n", buffer );
    //    }
    //    if( ok )
    //    {
    //        ok &= mServerSocket.sendTcpData( buffer );
    //    }
    //}
    //if( ok )
    //{
    //    ok &= mServerSocket.closeConnection( SOCK_STREAM );
    //}
    //if( ok )
    //{
    //    ok &= mServerSocket.stopWaiting( );
    //}

    return ok;
}

bool Q4SServerProtocol::manageUdpConnection( )
{
    bool                ok = true;


    if( ok )
    {
        ok &= mServerSocket.waitForConnections( SOCK_DGRAM );
    }
    if( ok )
    {
        marrthrDataHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    }

    //char                buffer[ 65536 ];
    //
    //ok &= mServerSocket.receiveUdpData( buffer, sizeof( buffer ) );
    //printf( "Received: <%s>\n", buffer );

    return ok;
}

DWORD WINAPI Q4SServerProtocol::manageTcpReceivedDataFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageTcpReceivedData( );
}

DWORD WINAPI Q4SServerProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SServerProtocol::manageTcpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    while( ok ) 
    {
        ok &= mServerSocket.receiveTcpData( buffer, sizeof( buffer ) );
        std::string message = buffer;
        mReceivedMessages.addMessage ( message );
        printf( "Received: <%s>\n", buffer );
    }


    return ok;
}

bool Q4SServerProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    ok &= mServerSocket.receiveUdpData( buffer, sizeof( buffer ) );
    std::string message = buffer;
    mReceivedMessages.addMessage ( message );
    printf( "Received: <%s>\n", buffer );

    return ok;
}
