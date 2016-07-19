#include "Q4SServerProtocol.h"

#include "ETime.h"
#include "Q4SMathUtils.h"

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

    ok &= openConnectionListening();

    return ok;
}

void Q4SServerProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SServerProtocol::clear()
{
}

bool Q4SServerProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpConnectionsFn, ( void* ) this, 0, 0 );
    //marrthrListenHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpConnectionsFn, ( void* ) this, 0, 0 );
    mServerSocket.waitForUdpConnections( );
    marrthrListenHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SServerProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        WaitForMultipleObjects( 2, marrthrListenHandle, true, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SServerProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mServerSocket.closeConnection( SOCK_STREAM );
        ok &= mServerSocket.closeConnection( SOCK_DGRAM );
        WaitForMultipleObjects( 2, marrthrDataHandle, true, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


// State managing functions.

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

        unsigned long                   timeStamp = 0;
        std::vector< unsigned long >    arrSentPingTimestamps;
        Q4SMessageInfo                  messageInfo;
        std::string                     pattern;
        float                           latency;
        std::vector< float >            arrPingLatencies;
        float                           jitter;
        std::vector< unsigned long >    arrReceivedPingTimestamps;
        std::vector< float >            arrPingJitters;

        if( ok )
        {
            for( j = 0; j < jmax; j++ )
            {
                timeStamp = ETime_getTime( );
                sprintf_s( buffer, "PING %d %d", j, timeStamp );
                ok &= mServerSocket.sendUdpData( buffer );
                arrSentPingTimestamps.push_back( timeStamp );
                Sleep( TIME_BETWEEN_PINGS );
            }

            Sleep( TIME_START_CALC );

            // Latency calculation.
            for( j = 0; j < jmax; j++ )
            {
                sprintf_s( buffer, "200 OK %d", j );
                pattern = buffer;
                if( mReceivedMessages.readMessage( pattern, messageInfo ) == true )
                {
                    latency = ( messageInfo.timeStamp - arrSentPingTimestamps[ j ] ) / 2.0f;
                    arrPingLatencies.push_back( latency );
                    printf( "PING %d latency: %.2f\n", j, latency );
                }
            }
            printf( "Latencies median: %.3f\n", EMathUtils_median( arrPingLatencies ) );

            // Jitter calculation.
            for( j = 0; j < jmax; j++ )
            {
                sprintf_s( buffer, "PING %d", j );
                pattern = buffer;
                if( mReceivedMessages.readMessage( pattern, messageInfo ) == true )
                {
                    arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
                    if( j > 0 )
                    {
                        jitter = ( arrReceivedPingTimestamps[ j ] - arrReceivedPingTimestamps[ j - 1 ] );
                        arrPingJitters.push_back( jitter );
                        printf( "PING %d ET: %.2f\n", j, jitter );
                    }
                }
            }
            float ets = EMathUtils_mean( arrPingJitters );
            printf( "Latencies mean ET: %.3f; jitter: %.3f\n", ets, ets - TIME_BETWEEN_PINGS );
        }
    }

    return ok;
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

void Q4SServerProtocol::end()
{
    closeConnections();
}


// Incoming connection managing functions.

DWORD WINAPI Q4SServerProtocol::manageTcpConnectionsFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageTcpConnection( );
}

//DWORD WINAPI Q4SServerProtocol::manageUdpConnectionsFn( LPVOID lpData )
//{
//    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
//    return q4sCP->manageUdpConnection( );
//}

bool Q4SServerProtocol::manageTcpConnection( )
{
    bool                ok = true;

    if( ok )
    {
        ok &= mServerSocket.startTcpListening( );
    }

    while( ok )
    {
        ok &= mServerSocket.waitForTcpConnection( );
        if( ok )
        {
            ManageTcpConnectionsFnInfo  fnInfo;
            fnInfo.pThis = this;
            fnInfo.connId = 0;
            marrthrDataHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpReceivedDataFn, ( void* ) &fnInfo, 0, 0 );
        }
    }

    return ok;
}

//bool Q4SServerProtocol::manageUdpConnection( )
//{
//    bool                ok = true;
//    
//    if( ok )
//    {
//        ok &= mServerSocket.waitForConnections( SOCK_DGRAM );
//    }
//    if( ok )
//    {
//        marrthrDataHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
//    }
//
//    return ok;
//}


// Received data managing functions.

DWORD WINAPI Q4SServerProtocol::manageTcpReceivedDataFn( LPVOID lpData )
{
    ManageTcpConnectionsFnInfo* q4sCFI = ( ManageTcpConnectionsFnInfo* )lpData;
    return q4sCFI->pThis->manageTcpReceivedData( q4sCFI->connId );
}

DWORD WINAPI Q4SServerProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SServerProtocol* q4sCP = ( Q4SServerProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SServerProtocol::manageTcpReceivedData( int connId )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    while( ok ) 
    {
        ok &= mServerSocket.receiveTcpData( connId, buffer, sizeof( buffer ) );
        if( ok )
        {
            std::string message = buffer;
            mReceivedMessages.addMessage ( message );
            printf( "Received: <%s>\n", buffer );
        }
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

        if( ok )
        {
            unsigned long actualTimeStamp = ETime_getTime();

            std::string message = udpBuffer;

            int pingNumber = 0;
            unsigned long receivedTimeStamp = 0;

            // Comprobar que es un ping
            if ( isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {
                printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, receivedTimeStamp);

                // mandar respuesta del ping
                char buffer[ 256 ];
                printf( "Ping responsed %d\n", pingNumber);
                sprintf_s( buffer, "200 OK %d", pingNumber );
                ok &= mServerSocket.sendUdpData( buffer );
            
                // encolar el ping y el timestamp para el calculo del jitter
                mReceivedMessages.addMessage(message, receivedTimeStamp);
            }
            else
            {
                // encolar el 200 ok y el timestamp actual para el calculo de la latencia
                mReceivedMessages.addMessage(message, actualTimeStamp);
            }

            printf( "Received Udp: <%s>\n", udpBuffer );
        }
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