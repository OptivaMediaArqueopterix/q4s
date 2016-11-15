#include "Q4SClientProtocol.h"

#include <stdio.h>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SClientConfigFile.h"
#include "EKey.h"
#include "Q4SMessage.h"
#include "Q4SMessageTools.h"

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

    if (ok)
    {
        ok &= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnections();
    }

    return ok;
}

void Q4SClientProtocol::done()
{
    closeConnections();
    mReceivedMessages.done( );
}

void Q4SClientProtocol::clear()
{
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

    // launch received data managing threads.
    if( ok )
    {
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
        ok &= mClientSocket.closeConnection( SOCK_STREAM );
        ok &= mClientSocket.closeConnection( SOCK_DGRAM );
        WaitForMultipleObjects( 2, marrthrHandle, true, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


// State managing functions.

bool Q4SClientProtocol::begin()
{
    printf("METHOD: begin\n");

    bool ok = true;
    
    if( ok )
    {
        Q4SMessage message;
        message.init(Q4SMREQUESTORRESPOND_REQUEST, Q4SMTYPE_BEGIN, "myIP", q4SClientConfigFile.defaultTCPPort);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
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
        Q4SMessage message;
        message.init(Q4SMREQUESTORRESPOND_REQUEST, Q4SMTYPE_READY, "myIP", q4SClientConfigFile.defaultTCPPort);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok ) 
    {
        std::string message;
        mReceivedMessages.readFirst( message );
    }

    return ok;
}

bool Q4SClientProtocol::measure(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss)
{
    bool measureOk = true;

    measureOk = Q4SClientProtocol::measureStage0(maxLatency, maxJitter);
    if (measureOk)
    {
        measureOk = Q4SClientProtocol::measureStage1( minBandWith, maxPacketLoss);
    }

    return measureOk;
}

void Q4SClientProtocol::continuity(float maxLatency, float maxJitter, float minBandWith, float maxPacketLoss)
{
    bool stop = false;
    bool measureOk = true;

    while ( !stop )
    {
        measureOk = measure(maxLatency, maxJitter, minBandWith, maxPacketLoss);
        stop = !measureOk;
    }
}

void Q4SClientProtocol::bwidth()
{
    printf("METHOD: bwidth TODO\n");
}

void Q4SClientProtocol::cancel()
{
    printf("METHOD: cancel TODO\n");
    mClientSocket.sendTcpData( "CANCEL" );
}

//--private:-------------------------------------------------------------------------------

bool Q4SClientProtocol::measureStage0(float maxLatency, float maxJitter)
{
    printf("METHOD: measureStage0\n");

    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
    float latency;
    float jitter;

    if (ok)
    {
        // Send regular pings
        ok &= sendRegularPings(arrSentPingTimestamps);
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
        // Wait the established time to start calculation
        Sleep( (DWORD)q4SClientConfigFile.timeStartCalc);

        // Calculate Latency
        calculateLatency(arrSentPingTimestamps, latency, true, q4SClientConfigFile.showMeasureInfo);

        // Calculate Jitter
        calculateJitter(jitter, true, q4SClientConfigFile.showMeasureInfo);

        // Check latency and jitter limits
        ok &= checkLatencyAndJitter(latency, jitter, maxLatency, maxJitter);
    }
    
    return ok;
}

bool Q4SClientProtocol::sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps)
{
    bool ok = true;

    Q4SMessage message;
    unsigned long timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = 20;

    for ( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        timeStamp = ETime_getTime( );
        arrSentPingTimestamps.push_back( timeStamp );

        // Prepare message and send
        message.init(Q4SMREQUESTORRESPOND_REQUEST, Q4SMTYPE_PING,"myIp", q4SClientConfigFile.defaultUDPPort, pingNumber, timeStamp);
        ok &= mClientSocket.sendUdpData( message.getMessageCChar() );

        // Wait the established time between pings
        Sleep( (DWORD)q4SClientConfigFile.timeBetweenPings );
    }

    return ok;
}

void Q4SClientProtocol::calculateLatency(std::vector<unsigned long> &arrSentPingTimestamps, float &latency, bool showResult, bool showMeasureInfo)
{
    char messagePattern[ 256 ];
    std::string pattern;
    Q4SMessageInfo messageInfo;
    std::vector<unsigned long> arrPingLatencies;
    unsigned long actualPingLatency;
    int pingNumber = 0;
    int pingNumberToSend = 20;

    // Prepare for Latency calculation
    for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Generate pattern
        sprintf_s( messagePattern, "200 OK %d", pingNumber );
        pattern = messagePattern;

        if( mReceivedMessages.readMessage( pattern, messageInfo, true ) == true )
        {
            // Actual ping latency calculation
            actualPingLatency = messageInfo.timeStamp - arrSentPingTimestamps[ pingNumber ];

            // Latency store
            arrPingLatencies.push_back( actualPingLatency );

            if (showMeasureInfo)
            {
                printf( "PING %d actual ping latency: %d\n", pingNumber, actualPingLatency );
            }
        }
    }

    // Latency calculation
    latency = EMathUtils_median( arrPingLatencies ) / 2.0f;
    if (showResult)
    {
        printf( "Latency: %.3f\n", latency );
    }
}

void Q4SClientProtocol::calculateJitter(float &jitter, bool showResult, bool showMeasureInfo)
{
        int pingNumber = 0;
        int pingNumberToSend = 20;
        char messagePattern[ 256 ];
        std::string pattern;
        Q4SMessageInfo messageInfo;
        unsigned long actualPingTimeWithPrevious;
        std::vector<float> arrPingJitters;
        std::vector<unsigned long> arrReceivedPingTimestamps;

        // Prepare for Jitter calculation
        for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
        {
            // Generate pattern
            sprintf_s( messagePattern, "PING %d", pingNumber );
            pattern = messagePattern;

            if( mReceivedMessages.readPingMessage( pingNumber, messageInfo, true ) == true )
            {
                arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
                if( pingNumber > 0 )
                {
                    // Actual time between this ping and previous calculation
                    actualPingTimeWithPrevious = ( arrReceivedPingTimestamps[ pingNumber ] - arrReceivedPingTimestamps[ pingNumber - 1 ] );

                    // Actual time between this ping and previous store
                    arrPingJitters.push_back( (float)actualPingTimeWithPrevious );

                    if (showMeasureInfo)
                    {
                        printf( "PING %d ET: %d\n", pingNumber, actualPingTimeWithPrevious );
                    }
                }
            }
        }

        // Jitter calculation
        float meanOfTimeWithPrevious = EMathUtils_mean( arrPingJitters );
        jitter = meanOfTimeWithPrevious - (float)q4SClientConfigFile.timeBetweenPings;

        if (showMeasureInfo)
        {
            printf( "Time With previous ping mean: %.3f\n", meanOfTimeWithPrevious );
        }

        if (showResult)
        {
            printf( "Jitter: %.3f\n", jitter );
        }
}

bool Q4SClientProtocol::checkLatencyAndJitter(float latency, float jitter, float maxLatency, float maxJitter)
{
    bool ok = true;

    if ( latency > maxLatency )
    {
        printf( "Lantecy limits not reached\n");
        ok = false;
    }

    if ( jitter > maxJitter)
    {
        printf( "Jitter limits not reached\n");
        ok = false;
    }

    return ok;
}

bool Q4SClientProtocol::measureStage1(float minBandWith, float maxPacketLoss)
{
    bool ok = true;

    // TODO measure bandwith and packet loss


    return ok;
}

// Received data managing functions.

DWORD WINAPI Q4SClientProtocol::manageTcpReceivedDataFn( LPVOID lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    bool ret = q4sCP->manageTcpReceivedData( );
    return ret;
}

DWORD WINAPI Q4SClientProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SClientProtocol* q4sCP = ( Q4SClientProtocol* )lpData;
    bool ret = q4sCP->manageUdpReceivedData( );
    return ret;
}

bool Q4SClientProtocol::manageTcpReceivedData( )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    
    while (ok) 
    {
        ok &= mClientSocket.receiveTcpData( buffer, sizeof( buffer ) );
        if( ok )
        {
            std::string message = buffer;
            mReceivedMessages.addMessage ( message );
            printf( "Received Tcp: <%s>\n", buffer );
        }
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

        if( ok )
        {
            unsigned long actualTimeStamp = ETime_getTime();

            std::string message = udpBuffer;

            int pingNumber = 0;
            unsigned long receivedTimeStamp = 0;

            // Comprobar que es un ping
            if ( Q4SMessageTools_isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {
                if (q4SClientConfigFile.showReceivedPingInfo)
                {
                    printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, receivedTimeStamp);
                }

                // mandar respuesta del ping
                char buffer[ 256 ];
                if (q4SClientConfigFile.showReceivedPingInfo)
                {
                    printf( "Ping responsed %d\n", pingNumber);
                }
                sprintf_s( buffer, "200 OK %d", pingNumber );
                ok &= mClientSocket.sendUdpData( buffer );
            
                // encolar el ping y el timestamp para el calculo del jitter
                mReceivedMessages.addMessage(message, actualTimeStamp);
            }
            else
            {
                // encolar el 200 ok y el timestamp actual para el calculo de la latencia
                mReceivedMessages.addMessage(message, actualTimeStamp);
            }

            if (q4SClientConfigFile.showReceivedPingInfo)
            {
                printf( "Received Udp: <%s>\n", udpBuffer );
            }
        }

        // Key management
        if (EKey_getKeyState(EK_C))
        {
            printf( "CANCEL key pressed\n");
            cancel();
            ok = false;
        }
    }

    return ok;
}