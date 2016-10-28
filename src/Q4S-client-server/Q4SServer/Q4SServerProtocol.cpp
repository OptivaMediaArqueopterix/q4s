#include "Q4SServerProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SServerConfigFile.h"

#define     DEFAULT_CONN_ID     1

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

    if (ok)
    {
        mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnectionListening();
    }

    if (ok)
    {
        ok &= mServerSocket.startAlertSender();
    }

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
    marrthrListenHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SServerProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        mServerSocket.stopWaiting( );
    }
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
        ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, "200 OK" );
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
        ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, "200 OK" );
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
        int     pingIndex = 0,
                pingMaxCount = 20;
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
            for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
            {
                timeStamp = ETime_getTime( );
                sprintf_s( buffer, "PING %d %d", pingIndex, timeStamp );
                ok &= mServerSocket.sendUdpData( DEFAULT_CONN_ID, buffer );
                arrSentPingTimestamps.push_back( timeStamp );
                Sleep( (DWORD)q4SServerConfigFile.timeBetweenPings );
            }

            Sleep( (DWORD)q4SServerConfigFile.timeStartCalc);

            // Latency calculation.
            for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
            {
                sprintf_s( buffer, "200 OK %d", pingIndex );
                pattern = buffer;
                if( mReceivedMessages.readMessage( pattern, messageInfo ) == true )
                {
                    latency = ( messageInfo.timeStamp - arrSentPingTimestamps[ pingIndex ] ) / 2.0f;
                    arrPingLatencies.push_back( latency );
                    printf( "PING %d latency: %.2f\n", pingIndex, latency );
                }
            }
            printf( "Latencies median: %.3f\n", EMathUtils_median( arrPingLatencies ) );

            // Jitter calculation.
            for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
            {
                sprintf_s( buffer, "PING %d", pingIndex );
                pattern = buffer;
                if( mReceivedMessages.readMessage( pattern, messageInfo ) == true )
                {
                    arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
                    if( pingIndex > 0 )
                    {
                        jitter = ( arrReceivedPingTimestamps[ pingIndex ] - arrReceivedPingTimestamps[ pingIndex - 1 ] );
                        arrPingJitters.push_back( jitter );
                        printf( "PING %d ET: %.2f\n", pingIndex, jitter );
                    }
                }
            }
            float ets = EMathUtils_mean( arrPingJitters );
            printf( "Latencies mean ET: %.3f; jitter: %.3f\n", ets, ets - q4SServerConfigFile.timeBetweenPings );
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
    mServerSocket.sendAlertData("ALERT");
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
    bool        ok = true;
    int         newConnId = 1;

    if( ok )
    {
        ok &= mServerSocket.startTcpListening( );
    }

    while( ok )
    {
        ok &= mServerSocket.waitForTcpConnection( newConnId );
        if( ok )
        {
            ManageTcpConnectionsFnInfo  fnInfo;
            fnInfo.pThis = this;
            fnInfo.connId = newConnId;
            marrthrDataHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpReceivedDataFn, ( void* ) &fnInfo, 0, 0 );
        }
        newConnId++;
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
    int                 connId;

    mServerSocket.startUdpListening( );

    while ( ok )
    {
        ok &= mServerSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ), connId );

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
                ok &= mServerSocket.sendUdpData( connId, buffer );
            
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