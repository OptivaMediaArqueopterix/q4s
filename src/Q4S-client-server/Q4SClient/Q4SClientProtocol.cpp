#include "Q4SClientProtocol.h"

#include "Q4SClientConfigFile.h"

#include "..\Q4SCommon\ETime.h"
#include "..\Q4SCommon\Q4SMathUtils.h"
#include "..\Q4SCommon\EKey.h"
#include "..\Q4SCommon\Q4SMessage.h"
#include "..\Q4SCommon\Q4SMessageTools.h"
#include "..\Q4SCommon\ObtainMyIP.h"

#include <stdio.h>
#include <sstream>

Q4SClientProtocol::Q4SClientProtocol ()
{
    clear();
}

Q4SClientProtocol::~Q4SClientProtocol ()
{
    done();
}

bool Q4SClientProtocol::init(unsigned long times, unsigned long milisecondsBetweenTimes)
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
        ok &= tryOpenConnectionsTimes(times, milisecondsBetweenTimes);
    }

	if (ok)
	{
		obtainMyIP(mIP);
	}

    return ok;
}

bool Q4SClientProtocol::tryOpenConnectionsTimes(unsigned long times, unsigned long milisecondsBetweenTimes)
{
	bool connected = false;

	unsigned long actualTime = 0;
	while ((actualTime<times) & !connected)
	{
		connected = openConnections();

		Sleep(milisecondsBetweenTimes);

		actualTime++;
	}

	return connected;
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

bool Q4SClientProtocol::ready(unsigned long stage)
{
    printf("METHOD: ready\n");

    bool ok = true;
    
    if( ok )
    {
        Q4SMessage message;
        message.initRequest(Q4SMTYPE_READY, mIP, q4SClientConfigFile.defaultTCPPort, false, 0, false, 0, true, stage);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok ) 
    {
        std::string message;
        mReceivedMessages.readFirst( message );
    }

    return ok;
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

bool Q4SClientProtocol::handshake(Q4SSDPParams &params)
{
    printf("----------Handshake Phase\n");

    printf("METHOD: begin\n");

    bool ok = true;
    
    if( ok )
    {
        Q4SMessage message;
        message.initRequest(Q4SMTYPE_BEGIN, mIP, q4SClientConfigFile.defaultTCPPort);
        ok &= mClientSocket.sendTcpData( message.getMessageCChar() );
    }

    if ( ok )
    {
        std::string message;
        ok &= mReceivedMessages.readFirst( message );
		ok &= Q4SMessageTools_is200OKMessage(message);
		ok &= Q4SSDP_parse(message, params);
		ok &= Q4SSDP_parsePublicServerAddress(message, params);
		ok &= Q4SSDP_parsePublicClientAddress(message, params);
    }

    return ok;
}

bool Q4SClientProtocol::negotiation(Q4SSDPParams params, Q4SMeasurementResult &results)
{
    printf("----------Negotiation Phase\n");

	bool ok = true;

	ok &= Q4SClientProtocol::ready(0);

    if( ok )
    {
		printf("MEASURING\n");

		Q4SMeasurementResult downResults;

		ok = Q4SClientProtocol::measureStage0(params, results, downResults, 20);
		if (ok)
		{
			printf( "END stage0\n");
			ok &= Q4SClientProtocol::ready(1);
			if (ok)
			{
				ok = Q4SClientProtocol::measureStage1(params, results, downResults);
			}
		}
	}

	if (ok)
	{
		mResults = results;
	}

	return ok;
}

void Q4SClientProtocol::continuity(Q4SSDPParams params)
{
    printf("----------Continuity Phase\n");

	bool stop = false;
    bool measureOk = true;

    while ( !stop )
    {
		printf("MEASURING\n");

        Q4SMeasurementResult results;
        Q4SMeasurementResult downResults;

		measureOk = Q4SClientProtocol::measureContinuity(params, results, downResults, 20);

        // TODO notificar del error al Server
        //stop = !measureOk;
        if (!measureOk)
        {
            //Alert
            std::string alertMessage = "";

            if ( results.latencyAlert )
            {
                alertMessage += "Latency: " + std::to_string((long double)results.values.latency);
            }
            if ( results.jitterAlert )
            {
                alertMessage += " Jitter: " + std::to_string((long double)results.values.jitter);
            }

            if ( results.packetLossAlert)
            {
                alertMessage += " PacketLoss: " + std::to_string((long double)results.values.packetLoss);
            }

            //printf(alertMessage.c_str());
        }
    }
}

//--private:-------------------------------------------------------------------------------

bool Q4SClientProtocol::measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
	Q4SMeasurementValues downMeasurements;

    if (ok)
    {
        // Send regular pings
		ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.negotiationTimeBetweenPingsUplink);
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
		Sleep(params.procedure.negotiationTimeBetweenPingsUplink * 10);

		// Calculate Latency
        calculateLatency(
			mReceivedMessages, 
			arrSentPingTimestamps, 
			results.values.latency, 
			pingsToSend, 
			q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Up: %.3f\n", results.values.latency );

        // Calculate Jitter
		float packetLoss = 0.f;
        calculateJitterStage0(
			mReceivedMessages, 
			results.values.jitter,
			params.procedure.negotiationTimeBetweenPingsUplink, 
			pingsToSend,
			q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Up: %.3f\n", results.values.jitter );
	}

    if ( ok ) 
    {
		ok &= interchangeMeasurementProcedure(downMeasurements, results);
		printf( "MEASURING RESULT - Latency Down: %.3f\n", downMeasurements.latency );
		printf( "MEASURING RESULT - Jitter Down: %.3f\n", downMeasurements.jitter );
    }
    
	if ( ok ) 
    {
		downResults.values = downMeasurements;
		ok &= checkStage0(params.latency, params.jitterUp, params.latency, params.jitterDown, results, downResults);
    }

	if (!ok)
	{
		showCheckMessage(results, downResults);
	}

    return ok;
}

bool Q4SClientProtocol::interchangeMeasurementProcedure(Q4SMeasurementValues &downMeasurements, Q4SMeasurementResult results)
{
	bool ok = true;

    if ( ok ) 
    {
		// Send Info Ping with sequenceNumber 0
		Q4SMessage infoPingMessage;
		ok &= infoPingMessage.initPing(mIP, q4SClientConfigFile.defaultUDPPort, 0, 0, results.values);
        ok &= mClientSocket.sendTcpData( infoPingMessage.getMessageCChar() );
    }
    
    if (ok)
    {
		// Wait to recive the measurements Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readPingMessage( 0, messageInfo, true );
		if (ok)
		{
			ok &= Q4SMeasurementValues_parse(messageInfo.message, downMeasurements);
			if (!ok)
			{
				printf( "ERROR:Interchange Read measurements fail\n");
			}
		}
		else
		{
            printf( "ERROR:Interchange Read PING fail\n");
            printf( "Messages:\n");
			std::string toPrint;
			while (mReceivedMessages.readFirst(toPrint))
			{
				printf(toPrint.c_str());
			}
		}
    }

	return ok;
}

bool Q4SClientProtocol::measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
	Q4SMeasurementValues downMeasurements;

    if (ok)
    {
        // Send regular pings
		ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.continuityTimeBetweenPingsUplink);
    }
    
    if(!ok)
    {
        printf( "ERROR:sendUdpData PING.\n" );
    }

    if (ok)
    {
		Sleep(params.procedure.negotiationTimeBetweenPingsUplink * 10);

        // Calculate Latency
        calculateLatency(
			mReceivedMessages, 
			arrSentPingTimestamps, 
			results.values.latency, 
			pingsToSend, 
			q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Up: %.3f\n", results.values.latency );

        // Calculate Jitter
        calculateJitterAndPacketLossContinuity(
			mReceivedMessages, 
			results.values.jitter, 
			params.procedure.continuityTimeBetweenPingsUplink, 
			pingsToSend,
			results.values.packetLoss,
			q4SClientConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Up: %.3f\n", results.values.jitter );
        printf( "MEASURING RESULT - PacketLoss Up: %.3f\n", results.values.packetLoss );
	}

	if (ok)
	{
		ok &= interchangeMeasurementProcedure(downMeasurements, results);
        printf( "MEASURING RESULT - Latency Down: %.3f\n", downMeasurements.latency );
        printf( "MEASURING RESULT - Jitter Down: %.3f\n", downMeasurements.jitter );
		printf( "MEASURING RESULT - PacketLoss Down: %.3f\n", downMeasurements.packetLoss );
	}

	if (ok)
	{
        // Check 
		downResults.values = downMeasurements;

		ok &= Q4SCommonProtocol::checkContinuity(
			params.latency, params.jitterUp, params.packetLossUp, 
			params.latency, params.jitterDown, params.packetLossDown,
			results,
			downResults);
    }
    
	if (!ok)
	{
		showCheckMessage(results, downResults);
	}

	return ok;
}

bool Q4SClientProtocol::sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings)
{
    bool ok = true;

    Q4SMessage message;
    unsigned long timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = pingsToSend;

    for ( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        timeStamp = ETime_getTime( );
        arrSentPingTimestamps.push_back( timeStamp );

        // Prepare message and send
        message.initPing(mIP, q4SClientConfigFile.defaultUDPPort, pingNumber, timeStamp);
        ok &= mClientSocket.sendUdpData( message.getMessageCChar() );

        // Wait the established time between pings
        Sleep( (DWORD)timeBetweenPings );
    }

    return ok;
}

bool Q4SClientProtocol::measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &downResults)
{
    bool ok = true;

	Q4SMeasurementValues downMeasurements;

    printf( "Starting:measureStage1.\n" );

    Q4SMessage message;
    unsigned long initialTimeStamp = ETime_getTime( );
	unsigned long sequenceNumber = 0;
	while(ok && (ETime_getTime() < initialTimeStamp + params.procedure.bandwidthTime))
	{
		ok &= message.initRequest(Q4SMTYPE_BWIDTH, mIP, q4SClientConfigFile.defaultUDPPort, true, sequenceNumber, true, ETime_getTime());
		ok &= mClientSocket.sendUdpData( message.getMessageCChar() );

		sequenceNumber++;
	}

	if (ok)
    {
		calculateBandwidthStage1(sequenceNumber, params.procedure.bandwidthTime, results.values.bandwidth);
		printf( "MEASURING RESULT - Bandwidth Up: %0.2f\n", results.values.bandwidth );
	}

	if (ok)
	{
		// Calculate PacketLoss
        bool okCalculated = calculatePacketLossStage1(mReceivedMessages, results.values.packetLoss);
		if (!okCalculated)
		{
			printf( "PacketLoss Calculation Error");
		}

        printf( "MEASURING RESULT - PacketLoss Up: %.3f\n", results.values.packetLoss );
	}

    if(!ok)
    {
        printf( "ERROR:sendUdpData BWidth.\n" );
    }

	if (ok)
	{
		ok &= interchangeMeasurementProcedure(downMeasurements, results);
		printf( "MEASURING RESULT - Bandwidth Down: %.3f\n", downMeasurements.bandwidth);
		printf( "MEASURING RESULT - PacketLoss Down: %.3f\n", downMeasurements.packetLoss );
	}

	if (ok)
	{
		// Check
		downResults.values = downMeasurements;
		ok &= checkStage1(params.bandWidthUp, params.packetLossUp, params.bandWidthDown, params.packetLossDown, results, downResults);
	}

	if (!ok)
	{
		showCheckMessage(results, downResults);
	}

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
				char reasonPhrase[ 256 ];
                if (q4SClientConfigFile.showReceivedPingInfo)
                {
                    printf( "Ping responsed %d\n", pingNumber);
                }
				Q4SMessage message200;
                sprintf_s( reasonPhrase, "OK %d", pingNumber );
				ok &= message200.initResponse(Q4SRESPONSECODE_200, reasonPhrase);
				ok &= mClientSocket.sendUdpData( message200.getMessageCChar() );

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