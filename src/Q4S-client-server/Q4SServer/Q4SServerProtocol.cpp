#include "Q4SServerProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SServerConfigFile.h"
#include "EKey.h"
#include "Q4SMessage.h"
#include "Q4SMessageTools.h"
#include "..\Q4SCommon\Q4SSDPParams.h"
#include "..\Q4SCommon\ObtainMyIP.h"

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
	bool okReceivedMessages;
    if (ok)
    {
        okReceivedMessages= mReceivedMessages.init( );
    }

    if (ok)
    {
        ok &= openConnectionListening();
    }

    if (ok)
    {
        ok &= mServerSocket.startAlertSender();
    }

	if (ok)
	{
		obtainMyIP(mIP);
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
    lastAlertTimeStamp = ULONG_MAX;
	qosLevel = 0;
}

bool Q4SServerProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpConnectionsFn, ( void* ) this, 0, 0 );
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

bool Q4SServerProtocol::handshake(Q4SSDPParams &params)
{
    printf("----------Handshake Phase\n");

    printf("WAITING FOR BEGIN\n");
    std::string message;

    bool ok = true;
    
    if ( ok ) 
    {
        // Wait for a message
        mReceivedMessages.readFirst( message );
    }

    if (ok)
    {
        std::string pattern;
        pattern.assign( "BEGIN" );
        if ( message.substr( 0, pattern.size( ) ).compare( pattern ) != 0)
        {
            ok = false;
        }
		else
		{
			//ok = Q4SSDP_parsePublicClineServerAddress(message, params);
		}
    }
    
    if( ok )
    {
		Q4SMessage message200;

		params.qosLevelUp = 0;
		params.qosLevelDown = 0;
		if (q4SServerConfigFile.isReactive)
		{
			params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_REACTIVE;
		}
		else
		{
			params.q4SSDPAlertingMode = Q4SSDPALERTINGMODE_Q4SAWARENETWORK;
		}
		params.alertPause = q4SServerConfigFile.alertPause;
		params.recoveryPause = q4SServerConfigFile.recoveryPause;
		params.latency = q4SServerConfigFile.latency;
		params.jitterUp = q4SServerConfigFile.jitterUp;
		params.jitterDown = q4SServerConfigFile.jitterDown;
		params.bandWidthUp = q4SServerConfigFile.bandwidthUp;
		params.bandWidthDown = q4SServerConfigFile.bandwidthDown;
		params.packetLossUp = q4SServerConfigFile.packetLossUp;
		params.packetLossDown = q4SServerConfigFile.packetLossDown;
		params.procedure.negotiationTimeBetweenPingsUplink = q4SServerConfigFile.timeBetweenPingsNegotiationUp;
		params.procedure.negotiationTimeBetweenPingsDownlink = q4SServerConfigFile.timeBetweenPingsNegotiationDown;
		params.procedure.continuityTimeBetweenPingsUplink = q4SServerConfigFile.timeBetweenPingsContinuityUp;
		params.procedure.continuityTimeBetweenPingsDownlink = q4SServerConfigFile.timeBetweenPingsContinuityDown;
		params.procedure.bandwidthTime = q4SServerConfigFile.bandwidthTime;
		params.procedure.windowSizeLatencyCalcUplink= q4SServerConfigFile.numberOfPingsUp;
		params.procedure.windowSizeLatencyCalcDownlink= q4SServerConfigFile.numberOfPingsDown;
		params.procedure.windowSizePacketLossCalcUplink= q4SServerConfigFile.numberOfBwidthsUp;
		params.procedure.windowSizePacketLossCalcDownlink= q4SServerConfigFile.numberOfBwidthsDown;
		params.publicServerAddress = mIP;

		ok &= message200.init200OKBeginResponse(params);
		ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, message200.getMessageCChar());
    }

    return ok;
}

bool Q4SServerProtocol::negotiation(Q4SSDPParams params, Q4SMeasurementResult &results)
{
    printf("----------Negotiation Phase\n");

	bool ok = true;

	ok &= Q4SServerProtocol::ready();

	if (ok)
	{
		printf("MEASURING\n");

		Q4SMeasurementResult upResults;

		ok &= Q4SServerProtocol::measureStage0(params, results, upResults, 20);
		if (ok)
		{
			printf( "END stage0\n");
			ok &= Q4SServerProtocol::ready();
			if (ok)
			{
				ok &= Q4SServerProtocol::measureStage1(params, results, upResults);
			}
		}

        if (!ok)
        {
            std::string alertMessage;
			alertMessage.append("Latency: " + std::to_string((long double)results.values.latency));
			alertMessage.append(" Jitter: " + std::to_string((long double)results.values.jitter));
			alertMessage.append(" PacketLoss: " + std::to_string((long double)results.values.packetLoss));
			alertMessage.append(" BandWidth: " + std::to_string((long double)results.values.bandwidth));

            //Alert
            Q4SServerProtocol::alert(alertMessage);
        }

		// Check if CANCEL has been received
		if (mReceivedMessages.size() != 0)
		{
			Q4SMessageInfo message;
			std::string pattern;
			pattern.assign( "CANCEL" );
			bool cancelMessageReceived = mReceivedMessages.readMessage( pattern, message, false);
			if ( cancelMessageReceived)
			{
				printf("CANCEL received: %s\n", message.message.c_str());
				ok = false;
			}
		}
	}

	return ok;
}

void Q4SServerProtocol::continuity(Q4SSDPParams params)
{
    printf("----------Continuity Phase\n");

    bool stop = false;
    bool measureOk = true;

    while ( !stop )
    {
		printf("MEASURING\n");

        Q4SMeasurementResult results;
        Q4SMeasurementResult upResults;

		measureOk = Q4SServerProtocol::measureContinuity(params, results, upResults, 20);
        if (!measureOk)
        {
            //Alert
            std::string alertMessage = generateNotificationAlertMessage(params, upResults, results);

            alert(alertMessage);
        }
		else
		{
			// Recovery
			std::string alertMessage = generateNotificationAlertMessage(params, upResults, results);

			recovery(alertMessage);
		}
//        stop = !measureOk;
    }
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

    if (ok)
    {
        std::string patternReady;
        patternReady.assign( "READY" );
        if ( message.substr( 0, patternReady.size( ) ).compare( patternReady ) != 0)
        {
            ok = false;
        }
		else
		{
			std::string patternStage;
			patternStage.assign("Stage:");
			if ( message.find( patternStage) == std::string::npos)
			{
				ok = false;
			}
		}
    }

    if( ok )
    {
		Q4SMessage message200;
		ok &= message200.initResponse(Q4SRESPONSECODE_200, "OK");
		ok &= mServerSocket.sendTcpData( DEFAULT_CONN_ID, message200.getMessageCChar());
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

void Q4SServerProtocol::alert(std::string alertMessage)
{
    unsigned long actualTime = ETime_getTime();
    unsigned long timeFromLastAlert = actualTime - lastAlertTimeStamp;
    if ( timeFromLastAlert > q4SServerConfigFile.alertPause)
    {
		qosLevel++;

        lastAlertTimeStamp = actualTime;

        std::string message = "ALERT "+alertMessage;
        mServerSocket.sendAlertData(message.c_str());

        printf("METHOD: alert\n");
		printf("QOS Level: %d\n", qosLevel);
    }
}

void Q4SServerProtocol::recovery(std::string recoveryMessage)
{
	if (qosLevel == 0)
	{
		printf("No recovery send because QOS Level = 0\n", qosLevel);
	}
	else
	{
		if (lastAlertTimeStamp > recoveryTimeStamp)
		{
			recoveryTimeStamp = lastAlertTimeStamp;
		}

		unsigned long actualTime = ETime_getTime();
		unsigned long timeForRecovery = actualTime - recoveryTimeStamp;
		if ( timeForRecovery > q4SServerConfigFile.recoveryPause)
		{
			qosLevel--;

			recoveryTimeStamp = actualTime;

			std::string message = "RECOVERY " + recoveryMessage;
			mServerSocket.sendAlertData(message.c_str());

			printf("METHOD: recovery\n");
			printf("QOS Level: %d\n", qosLevel);
		}
	}
}

void Q4SServerProtocol::end()
{
    closeConnections();
}

//--private:-------------------------------------------------------------------------------

bool Q4SServerProtocol::measureStage0(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
	Q4SMeasurementValues upMeasurements;

    if ( ok ) 
    {
        // Wait to recive the first Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readPingMessage( 0, messageInfo, false );
    }

    if(!ok)
    {
        printf( "ERROR:PING 0 is not the first message.\n" );
    }

    if( ok )
    {
        // Send regular pings
		ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.negotiationTimeBetweenPingsDownlink);
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
			q4SServerConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Down: %.3f\n", results.values.latency );

        // Calculate Jitter
        calculateJitterStage0(
			mReceivedMessages, 
			results.values.jitter,
			params.procedure.negotiationTimeBetweenPingsDownlink, 
			pingsToSend,
			q4SServerConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Down: %.3f\n", results.values.jitter );
    }

    if ( ok ) 
    {
		ok &= interchangeMeasurementProcedure(upMeasurements, results);
		printf( "MEASURING RESULT - Latency Up: %.3f\n", upMeasurements.latency );
		printf( "MEASURING RESULT - Jitter Up: %.3f\n", upMeasurements.jitter );
    }

    if ( ok )
    {
		upResults.values = upMeasurements;

		ok &= checkStage0(params.latency, params.jitterUp, params.latency, params.jitterDown, upResults, results);
	}

	if (!ok)
	{
		showCheckMessage(upResults, results);
	}

    return ok;
}

bool Q4SServerProtocol::interchangeMeasurementProcedure(Q4SMeasurementValues &upMeasurements, Q4SMeasurementResult results)
{
	bool ok = true;

    if ( ok ) 
    {
		// Wait to recive the measurements Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readPingMessage( 0, messageInfo, true );
		if (ok)
		{
			ok &= Q4SMeasurementValues_parse(messageInfo.message, upMeasurements);
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

    if ( ok )
    {
		// Send Info Ping with sequenceNumber 0
		Q4SMessage infoPingMessage;
		ok &= infoPingMessage.initPing(mIP, q4SServerConfigFile.defaultUDPPort, 0, 0, results.values);
		ok &= mServerSocket.sendTcpData(DEFAULT_CONN_ID, infoPingMessage.getMessageCChar());
    }

	return ok;
}

bool Q4SServerProtocol::measureContinuity(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult &upResults, unsigned long pingsToSend)
{
    bool ok = true;

    std::vector<unsigned long> arrSentPingTimestamps;
	Q4SMeasurementValues upMeasurements;

    if ( ok ) 
    {
        // Wait to recive the first Ping
        Q4SMessageInfo  messageInfo;
        ok &= mReceivedMessages.readPingMessage( 0, messageInfo, false );
    }

    if(!ok)
    {
        printf( "ERROR:PING 0 is not the first message.\n" );
    }

    if( ok )
    {
        // Send regular pings
		ok &= sendRegularPings(arrSentPingTimestamps, pingsToSend, params.procedure.continuityTimeBetweenPingsDownlink);
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
			q4SServerConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Latency Down: %.3f\n", results.values.latency );

        // Calculate Jitter
        calculateJitterAndPacketLossContinuity(
			mReceivedMessages, 
			results.values.jitter,
			params.procedure.continuityTimeBetweenPingsDownlink, 
			pingsToSend,
			results.values.packetLoss,
			q4SServerConfigFile.showMeasureInfo);
        printf( "MEASURING RESULT - Jitter Down: %.3f\n", results.values.jitter );
        printf( "MEASURING RESULT - PacketLoss Down: %.3f\n", results.values.packetLoss );
    }

	if (ok)
	{
		ok &= interchangeMeasurementProcedure(upMeasurements, results);
        printf( "MEASURING RESULT - Latency Up: %.3f\n", upMeasurements.latency );
        printf( "MEASURING RESULT - Jitter Up: %.3f\n", upMeasurements.jitter );
		printf( "MEASURING RESULT - PacketLoss Up: %.3f\n", upMeasurements.packetLoss );
	}

	if (ok)
	{
        // Check
		upResults.values = upMeasurements;

		ok &= Q4SCommonProtocol::checkContinuity(
			params.latency, params.jitterUp, params.packetLossUp,
			params.latency, params.jitterDown, params.packetLossDown,
			upResults,
			results);
    }

	if (!ok)
	{
		showCheckMessage(upResults, results);
	}

    return ok;
}

bool Q4SServerProtocol::sendRegularPings(std::vector<unsigned long> &arrSentPingTimestamps, unsigned long pingsToSend, unsigned long timeBetweenPings)
{
    bool ok = true;

    Q4SMessage message;
    unsigned long timeStamp = 0;
    int pingNumber = 0;
    int pingNumberToSend = pingsToSend;

    for( pingNumber = 0; pingNumber < pingNumberToSend; pingNumber++ )
    {
        // Store the timestamp
        timeStamp = ETime_getTime( );
        arrSentPingTimestamps.push_back( timeStamp );

        // Prepare message and send
        message.initPing(mIP, q4SServerConfigFile.defaultUDPPort, pingNumber, timeStamp);
        ok &= mServerSocket.sendUdpData( DEFAULT_CONN_ID, message.getMessageCChar() );

        // Wait the established time between pings
        Sleep( (DWORD)timeBetweenPings );
    }

    return ok;
}

bool Q4SServerProtocol::measureStage1(Q4SSDPParams params, Q4SMeasurementResult &results, Q4SMeasurementResult upResults)
{
    bool ok = true;

	Q4SMeasurementValues upMeasurements;

    printf( "Starting:measureStage1.\n" );

    Q4SMessage message;
    unsigned long initialTimeStamp = ETime_getTime( );
	unsigned long sequenceNumber = 0;
	while(ok && (ETime_getTime() < initialTimeStamp + params.procedure.bandwidthTime))
	{
		ok &= message.initRequest(Q4SMTYPE_BWIDTH, mIP, q4SServerConfigFile.defaultUDPPort, true, sequenceNumber, true, ETime_getTime());
		ok &= mServerSocket.sendUdpData(DEFAULT_CONN_ID, message.getMessageCChar());

		sequenceNumber++;
	}

	if (ok)
    {
		calculateBandwidthStage1(sequenceNumber, params.procedure.bandwidthTime, results.values.bandwidth);
		printf( "MEASURING RESULT - BandWidth Down: %0.2f\n", results.values.bandwidth );
	}

	if (ok)
	{
		// Calculate PacketLoss
        bool okCalculated = calculatePacketLossStage1(mReceivedMessages, results.values.packetLoss);
		if (!okCalculated)
		{
			printf( "PacketLoss Calculation Error");
		}

		printf( "MEASURING RESULT - PacketLoss Down: %.3f\n", results.values.packetLoss );
	}

    if(!ok)
    {
        printf( "ERROR:sendUdpData BWidth.\n" );
    }

	if (ok)
	{
		ok &= interchangeMeasurementProcedure(upMeasurements, results);
		printf( "MEASURING RESULT - Bandwidth Up: %.3f\n", upMeasurements.bandwidth);
		printf( "MEASURING RESULT - PacketLoss Up: %.3f\n", upMeasurements.packetLoss );
	}

	if (ok)
	{
		// Check stage 1
		upResults.values = upMeasurements;

		ok &= checkStage1(params.bandWidthUp, params.packetLossUp, params.bandWidthDown, params.packetLossDown, upResults, results);
	}

	if (!ok)
	{
		showCheckMessage(upResults, results);
	}

    return ok;
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
            if ( Q4SMessageTools_isPingMessage(udpBuffer, &pingNumber, &receivedTimeStamp) )
            {
                if (q4SServerConfigFile.showReceivedPingInfo)
                {
                    printf( "Received Ping, number:%d, timeStamp: %d\n", pingNumber, receivedTimeStamp);
                }

                // mandar respuesta del ping
            	char reasonPhrase[ 256 ];
                if (q4SServerConfigFile.showReceivedPingInfo)
                {
                    printf( "Ping responsed %d\n", pingNumber);
                }
				Q4SMessage message200;
                sprintf_s( reasonPhrase, "OK %d", pingNumber );
				ok &= message200.initResponse(Q4SRESPONSECODE_200, reasonPhrase);
				ok &= mServerSocket.sendUdpData( connId, message200.getMessageCChar() );

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

        // Key management
        if (EKey_getKeyState(EK_A))
        {
            printf( "Alert Sended\n");
            std::string alertMessage;
            alertMessage= "Alert by keyboard";
            alert(alertMessage);
        }

    }

    return ok;
}