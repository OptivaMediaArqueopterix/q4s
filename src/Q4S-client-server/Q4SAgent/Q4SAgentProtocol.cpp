#include "Q4SAgentProtocol.h"

#include <stdio.h>
#include <vector>
#include <sstream>

#include "ETime.h"
#include "Q4SMathUtils.h"
#include "Q4SAgentConfigFile.h"

#define     DEFAULT_CONN_ID     1

Q4SAgentProtocol::Q4SAgentProtocol ()
{
    clear();
}

Q4SAgentProtocol::~Q4SAgentProtocol ()
{
    done();
}

bool Q4SAgentProtocol::init()
{
    // Prevention done call
    done();

    bool ok = true;

    mReceivedMessages.init( );

    ok &= openConnectionListening();

    return ok;
}

void Q4SAgentProtocol::done()
{
    closeConnectionListening();
    mReceivedMessages.done( );
}

void Q4SAgentProtocol::clear()
{
}

bool Q4SAgentProtocol::openConnectionListening()
{
    bool ok = true;

    marrthrListenHandle = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
    
    return ok;
}

void Q4SAgentProtocol::closeConnectionListening()
{
    bool ok = true;

    if( ok )
    {
        mAgentSocket.stopWaiting( );
    }
    if( ok )
    {
        WaitForSingleObject( marrthrListenHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}

void Q4SAgentProtocol::closeConnections()
{
    bool ok = true;

    if( ok )
    {
        ok &= mAgentSocket.closeConnection( SOCK_DGRAM );
        WaitForSingleObject( marrthrDataHandle, INFINITE );
    }

    if( !ok )
    {
        //TODO: launch error
        printf( "Error closing connections.\n" );
    }
}


// State managing functions.

bool Q4SAgentProtocol::listen()
{
    printf("METHOD: begin\n");
    std::string message;

    bool ok = true;
    
    if ( ok ) 
    {
        mReceivedMessages.readFirst( message );
    }

    return ok;
}


void Q4SAgentProtocol::end()
{
    closeConnections();
}


// Incoming connection managing functions.

//DWORD WINAPI Q4SAgentProtocol::manageUdpConnectionsFn( LPVOID lpData )
//{
//    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
//    return q4sCP->manageUdpConnection( );
//}

//bool Q4SAgentProtocol::manageUdpConnection( )
//{
//    bool                ok = true;
//    
//    if( ok )
//    {
//        ok &= mAgentSocket.waitForConnections( SOCK_DGRAM );
//    }
//    if( ok )
//    {
//        marrthrDataHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpReceivedDataFn, ( void* ) this, 0, 0 );
//    }
//
//    return ok;
//}


// Received data managing functions.

DWORD WINAPI Q4SAgentProtocol::manageUdpReceivedDataFn( LPVOID lpData )
{
    Q4SAgentProtocol* q4sCP = ( Q4SAgentProtocol* )lpData;
    return q4sCP->manageUdpReceivedData( );
}

bool Q4SAgentProtocol::manageUdpReceivedData( )
{
    bool                ok = true;
    char                udpBuffer[ 65536 ];
    int                 connId;

    mAgentSocket.startUdpListening( );

    while ( ok )
    {
        ok &= mAgentSocket.receiveUdpData( udpBuffer, sizeof( udpBuffer ), connId );

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
                ok &= mAgentSocket.sendUdpData( connId, buffer );
            
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

bool Q4SAgentProtocol::isPingMessage(std::string message, int *pingNumber, unsigned long *timeStamp)
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