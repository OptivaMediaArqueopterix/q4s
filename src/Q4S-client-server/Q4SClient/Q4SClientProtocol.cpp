#include "Q4SClientProtocol.h"

#include "ETime.h"

#include <stdio.h>
#include <vector>
#include <sstream>

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

    bool            ok = true;
    int             j = 0,
                    jmax = 20;
    char            buffer[ 256 ];
    unsigned long   timeStamp = 0;
    std::vector< unsigned long >    arrPings;
    Q4SMessageInfo                  messageInfo;
    std::string                     pattern;
    unsigned long   latency;

    if( ok )
    {
        for( j = 0; j < jmax; j++ )
        {
            unsigned long timeStamp = ETime_getTime( );
            printf( "Ping %d at %d\n", j, timeStamp );
            sprintf_s( buffer, "PING %d", j, timeStamp );
            ok &= mClientSocket.sendUdpData( buffer );
            arrPings.push_back( timeStamp );
            Sleep( 200 );
        }

        Sleep( 2000 );

        for( j = 0; j < jmax; j++ )
        {
            sprintf_s( buffer, "PING %d", j );
            pattern = buffer;
            if( mReceivedMessages.readMessage( pattern, messageInfo ) == true )
            {
                latency = messageInfo.timeStamp - arrPings[ j ];
                printf( "PING %d latency: %d\n", j, latency );
            }
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
    
    while (ok) 
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
    char                udpBuffer[ 65536 ];

    while ( ok )
    {
        ok &= mClientSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ) );
        std::string message = udpBuffer;

        int pingNumber = 0;
        unsigned long timeStamp = 0;

        // Comprobar que es un ping
        if ( isPingMessage(udpBuffer, &pingNumber) )
        {
            printf( "Received Ping, number:%d\n", pingNumber);

            // encolar el ping y el timestamp
            mReceivedMessages.addMessage(message, ETime_getTime());

            // mandar respuesta del ping
            char buffer[ 256 ];
            printf( "Ping responsed %d\n", pingNumber);
            sprintf_s( buffer, "200 OK %d", pingNumber );
            ok &= mClientSocket.sendUdpData( buffer );
        }

        printf( "Received Udp: <%s>\n", udpBuffer );
    }

    return ok;
}

bool Q4SClientProtocol::isPingMessage(std::string message, int *pingNumber)
{
    bool ok = true;

    // Convert message to a stringstream 
    std::istringstream messageStream (message);

    std::string method;

    // Get method from message
    if ( ok )
    {
        std::getline(messageStream, method, ' ');
        // Check if method is ping
        if ( method.compare("PING") != 0)
        {
            ok = false;
        }
    }

    // Get pingNumberfrom message
    if ( ok )
    {
        std::string stringPingNumber;
        std::getline(messageStream, stringPingNumber, ' ');

        *pingNumber = atoi(stringPingNumber.c_str());       
    }

    return ok;
}

