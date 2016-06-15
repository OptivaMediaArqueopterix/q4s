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

    ok &= openConnections();

    return ok;
}

void Q4SServerProtocol::done()
{
    closeConnections();
}

bool Q4SServerProtocol::openConnections()
{
    bool ok = true;

    HANDLE arrthrHandle[ 2 ];

    arrthrHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpConnectionsFn, ( void* ) this, 0, 0 );
    arrthrHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpConnectionsFn, ( void* ) this, 0, 0 );
    WaitForMultipleObjects( 2, arrthrHandle, true, INFINITE );
    
    return ok;
}

void Q4SServerProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        WaitForMultipleObjects( 2, marrthrHandle, true, INFINITE );
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

    bool ok = true;
    
    if( ok )
    {
        ok &= mServerSocket.sendTcpData( 
                "BEGIN"
                );
        //ok &= mClientSocket.sendUdpData( 
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        "Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba Tooooma prueba "
        //        );
    }

    if ( ok ) 
    {
        std::string message;
        while( !mReceivedMessages.readFirst(message))
        {
            //TODO: cambiar espera activa por pasiva
        }
    }

    return ok;
}

void Q4SServerProtocol::ready()
{
    printf("METHOD: ready\n");
}

void Q4SServerProtocol::ping()
{
    printf("METHOD: ping\n");
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

    //char                buffer[ 65536 ];
    //
    //ok &= mServerSocket.receiveUdpData( buffer, sizeof( buffer ) );
    //printf( "Received: <%s>\n", buffer );

    return ok;
}
