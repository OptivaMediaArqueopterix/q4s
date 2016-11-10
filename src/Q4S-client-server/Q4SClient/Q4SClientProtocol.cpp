#include "Q4SClientProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SClientConfigFile.h"

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

void Q4SClientProtocol::end()
{
    closeConnections();
}

bool Q4SClientProtocol::measureStage0(float maxLatency, float maxJitter)
{
    printf("METHOD: measureStage0\n");

    bool ok = true;
    int pingNumber = 0;
    int pingNumberToSend = 20;
    std::vector<unsigned long> arrSentPingTimestamps;
    std::vector<unsigned long> arrReceivedPingTimestamps;
    float latency;
    float jitter;

    if (ok)
    {
        char messageToSend[ 256 ];
        unsigned long timeStamp = 0;

        for ( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
        {
            // Store the timestamp
            timeStamp = ETime_getTime( );
            arrSentPingTimestamps.push_back( timeStamp );

            // Prepare message and send
            sprintf_s( messageToSend, "PING %d %d", pingNumber, timeStamp );
            ok &= mClientSocket.sendUdpData( messageToSend );

            // Wait the established time between pings
            Sleep( (DWORD)q4SClientConfigFile.timeBetweenPings );
        }
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok) 
    {
        // Wait the established time to start calculation
        Sleep( (DWORD)q4SClientConfigFile.timeStartCalc);
    }

    if (ok)
    {
        char messagePattern[ 256 ];
        std::string pattern;
        Q4SMessageInfo messageInfo;
        std::vector<unsigned long> arrPingLatencies;
        std::vector<float> arrPingJitters;
        unsigned long actualPingLatency;
        unsigned long actualPingTimeWithPrevious;

        // Prepare for Latency calculation
        for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
        {
            // Generate pattern
            sprintf_s( messagePattern, "200 OK %d", pingNumber );
            pattern = messagePattern;

            if( mReceivedMessages.readMessageAndPop( pattern, messageInfo ) == true )
            {
                // Actual ping latency calculation
                actualPingLatency = messageInfo.timeStamp - arrSentPingTimestamps[ pingNumber ];

                // Latency store
                arrPingLatencies.push_back( actualPingLatency );

                if (q4SClientConfigFile.showMeasureInfo)
                {
                    printf( "PING %d actual ping latency: %d\n", pingNumber, actualPingLatency );
                }

            }
        }

        // Latency calculation
        latency = EMathUtils_median( arrPingLatencies ) / 2.0f;
        printf( "Latency: %.3f\n", latency );

        // Prepare for Jitter calculation
        for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
        {
            // Generate pattern
            sprintf_s( messagePattern, "PING %d", pingNumber );
            pattern = messagePattern;

            if( mReceivedMessages.readMessageAndPop( pattern, messageInfo ) == true )
            {
                arrReceivedPingTimestamps.push_back( messageInfo.timeStamp );
                if( pingNumber > 0 )
                {
                    // Actual time between this ping and previous calculation
                    actualPingTimeWithPrevious = ( arrReceivedPingTimestamps[ pingNumber ] - arrReceivedPingTimestamps[ pingNumber - 1 ] );

                    // Actual time between this ping and previous store
                    arrPingJitters.push_back( (float)actualPingTimeWithPrevious );

                    if (q4SClientConfigFile.showMeasureInfo)
                    {
                        printf( "PING %d ET: %d\n", pingNumber, actualPingTimeWithPrevious );
                    }
                }
            }
        }

        // Jitter calculation
        float meanOfTimeWithPrevious = EMathUtils_mean( arrPingJitters );
        jitter = meanOfTimeWithPrevious - (float)q4SClientConfigFile.timeBetweenPings;
        printf( "Time With previous ping mean: %.3f\n", meanOfTimeWithPrevious );
        printf( "Jitter: %.3f\n", jitter );
    }

    // Check latency and jitter limits
    if (ok)
    {
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
            if ( isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
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
    }

    return ok;
}

bool Q4SClientProtocol::isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp)
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

