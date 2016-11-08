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
    bool closedConnectionAlertSender = mServerSocket.closeConnection(SOCK_DGRAM);
    if (!closedConnectionAlertSender)
    {
        printf( "Error closing sender socket connection.\n" );
    }

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
    printf("WAITING FOR BEGIN\n");
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
    printf("WAITING FOR READY\n");
    std::string message;

    bool ok = true;

    if ( ok ) 
    {
        ok &= mReceivedMessages.readFirst( message );
    }

    if( ok )
    {
        ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, "200 OK" );
    }

    return ok;
}

bool Q4SServerProtocol::measure(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss)
{
    bool measureOk = true;

    printf("MEASURING\n");

    measureOk = Q4SServerProtocol::measureStage0(maxLatency, maxJitter);
    if (measureOk)
    {
        measureOk = Q4SServerProtocol::measureStage1( 500, 10);
    }

    return measureOk;
}

void Q4SServerProtocol::continuity(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss)
{
    bool stop = false;
    bool measureOk = true;

    while ( !stop )
    {
        measureOk = measure(maxLatency, maxJitter, minBandWith, maxPacketLoss);
        stop = !measureOk;
    }
}

bool Q4SServerProtocol::measureStage0(float maxLatency, float maxJitter)
{
    bool ok = true;

    int pingIndex = 0;
    int pingMaxCount = 20;
    std::vector<unsigned long> arrSentPingTimestamps;
    std::vector<unsigned long> arrReceivedPingTimestamps;
    float latency;
    float jitter;

    if ( ok ) 
    {
        // Wait to recive the first Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readMessage( std::string( "PING 0" ), messageInfo );
    }

    if( ok )
    {
        char messageToSend[ 256 ];
        unsigned long timeStamp = 0;

        for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
        {
            // Store the timestamp
            timeStamp = ETime_getTime( );
            arrSentPingTimestamps.push_back( timeStamp );

            // Prepare message and send
            sprintf_s( messageToSend, "PING %d %d", pingIndex, timeStamp );
            ok &= mServerSocket.sendUdpData( DEFAULT_CONN_ID, messageToSend );

            // Wait the established time between pings
            Sleep( (DWORD)q4SServerConfigFile.timeBetweenPings );
        }
    }

    if (ok)
    {
        // Wait the established time to start calculation
        Sleep( (DWORD)q4SServerConfigFile.timeStartCalc);
    }

    if (ok)
    {
        char messagePattern[ 256 ];
        std::string pattern;
        Q4SMessageInfo messageInfo;
        std::vector<float> arrPingLatencies;
        std::vector<float> arrPingJitters;
        float actualPingLatency;
        float actualPingTimeWithPrevious;

        // Prepare for Latency calculation
        for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
        {
            // Generate pattern
            sprintf_s( messagePattern, "200 OK %d", pingIndex );
            pattern = messagePattern;

            if( mReceivedMessages.readMessageAndPop( pattern, messageInfo ) == true )
            {
                // Actual ping latency calculation
                actualPingLatency = (messageInfo.timeStamp - arrSentPingTimestamps[ pingIndex ])/ 2.0f;

                // Latency store
                arrPingLatencies.push_back( actualPingLatency );

                if (q4SServerConfigFile.showMeasureInfo)
                {
                    printf( "PING %d actual ping latency: %.3f\n", pingIndex, actualPingLatency );
                }
            }
            else
            {
                    printf( "PING %d message lost\n", pingIndex);
            }
        }

        // Latency calculation
        latency = EMathUtils_median( arrPingLatencies );
        printf( "MEASURING RESULT - Latency: %.3f\n", latency );

        // Prepare for Jitter calculation
        for( pingIndex = 0; pingIndex < pingMaxCount; pingIndex++ )
        {
            // Generate pattern
            sprintf_s( messagePattern, "PING %d", pingIndex );
            pattern = messagePattern;

            if( mReceivedMessages.readMessageAndPop( pattern, messageInfo ) == true )
            {
                arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
                if( pingIndex > 0 )
                {
                    // Actual time between this ping and previous calculation
                    actualPingTimeWithPrevious = (float)( arrReceivedPingTimestamps[ pingIndex ] - arrReceivedPingTimestamps[ pingIndex - 1 ] );

                    // Actual time between this ping and previous store
                    arrPingJitters.push_back( actualPingTimeWithPrevious );

                    if (q4SServerConfigFile.showMeasureInfo)
                    {
                        printf( "PING %d ET: %.3f\n", pingIndex, actualPingTimeWithPrevious );
                    }
                }
            }
        }

        // Jitter calculation
        float meanOfTimeWithPrevious = EMathUtils_mean( arrPingJitters );
        jitter = meanOfTimeWithPrevious - (float)q4SServerConfigFile.timeBetweenPings;
        printf( "MEASURING RESULT - Time With previous ping mean: %.3f\n", meanOfTimeWithPrevious );
        printf( "MEASURING RESULT - Jitter: %.3f\n", jitter );
    }

    // Check latency and jitter limits
    if (ok)
    {
        if ( latency > maxLatency )
        {
            ok = false;
        }

        if ( jitter > maxJitter)
        {
            ok = false;
        }
    }

    return ok;
}

bool Q4SServerProtocol::measureStage1(float minBandWith, float maxPacketLoss)
{
    bool ok = true;

    // TODO measure bandwith and packet loss

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
                if (q4SServerConfigFile.showReceivedPingInfo)
                {
                    printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, receivedTimeStamp);
                }

                // mandar respuesta del ping
                char buffer[ 256 ];
                if (q4SServerConfigFile.showReceivedPingInfo)
                {
                    printf( "Ping responsed %d\n", pingNumber);
                }
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

            if (q4SServerConfigFile.showReceivedPingInfo)
            {
                printf( "Received Udp: <%s>\n", udpBuffer );
            }
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