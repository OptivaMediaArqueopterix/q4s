#include "Q4SClientProtocol.h"

#include "Q4SClientSocket.h"

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


    return ok;
}

void Q4SClientProtocol::done()
{
}

void Q4SClientProtocol::begin()
{
    printf("METHOD: begin\n");
    
    Q4SClientSocket     clientSocket;
    Q4SSocket           q4sSocket;
    bool                ok = true;

    if( ok )
    {
        ok &= clientSocket.initializeSockets( );
    }
    if( ok )
    {
        ok &= clientSocket.connectToServer( &q4sSocket );
    }
    if( ok )
    {
        ok &= q4sSocket.sendData( "Tooooma prueba" );
    }
    if( ok )
    {
        char buffer[ 256 ];
        ok &= q4sSocket.receiveData( buffer, sizeof( buffer ) );
        printf( "Received: <%s>\n", buffer );
    }
    if( ok )
    {
        ok &= q4sSocket.shutDown( );
    }
}

void Q4SClientProtocol::ready()
{
    printf("METHOD: ready\n");
}

void Q4SClientProtocol::ping()
{
    printf("METHOD: ping\n");
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

