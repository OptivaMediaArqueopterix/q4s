#include "Q4SServerProtocol.h"

#include "ETime.h"

#include <stdio.h>
#include <vector>
#include <sstream>

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

    return ok;
}

bool Q4SServerProtocol::ping()
{
    printf("METHOD: ping\n");

    bool            ok = true;
    std::string     message;
    Q4SMessageInfo  messageInfo;

    if ( ok ) 
    {
        ok &= mReceivedMessages.readMessage( std::string( "PING 0" ), messageInfo );
    }

    if( ok )
    {
        int     j = 0,
                jmax = 20;
        char    buffer[ 256 ];
        unsigned long   timeStamp = 0;
        std::vector< unsigned long >    arrPings;
        Q4SMessageInfo                  messageInfo;
        std::string                     pattern;
        unsigned long   latency;

        if( ok )
        {
            for( j = 0; j < jmax; j++ )
            {
                timeStamp = ETime_getTime( );
                printf( "Ping %d at %d\n", j, timeStamp );
                sprintf_s( buffer, "PING %d %ld", j, timeStamp );
                ok &= mServerSocket.sendUdpData( buffer );
                arrPings[ j ] = timeStamp;
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
                    printf( "PING latency: %d", latency );
                }
            }
        }
    }

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

    //char                buffer[ 65536 ];

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
    char                udpBuffer[ 65536 ];

    while ( ok )
    {
        ok &= mServerSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ) );
        std::string message = udpBuffer;

        int pingNumber = 0;
        unsigned long timeStamp = 0;

        // Comprobar que es un ping
        if ( isPingMessage(udpBuffer, &pingNumber, &timeStamp) )
        {
            printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, timeStamp);

            // encolar el ping y el timestamp
            mReceivedMessages.addMessage(message, timeStamp);

            // mandar respuesta del ping
            char buffer[ 256 ];
            printf( "Ping responsed %d\n", pingNumber);
            sprintf_s( buffer, "200 OK %d", pingNumber );
            ok &= mServerSocket.sendUdpData( buffer );
        }

        printf( "Received Udp: <%s>\n", udpBuffer );
    }

    return ok;
}

bool Q4SServerProtocol::isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp)
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
    
    if ( ok )
    {
        std::string stringTimeStamp;
        std::getline(messageStream, stringTimeStamp, ' ');

        *timeStamp= atoi(stringTimeStamp.c_str());
    }

    return ok;

}