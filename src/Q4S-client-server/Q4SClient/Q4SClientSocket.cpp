#include "Q4SClientSocket.h"
#include "Q4SClientConfigFile.h"

Q4SClientSocket::Q4SClientSocket ()
{
    clear();
}

Q4SClientSocket::~Q4SClientSocket ()
{
    done();
}

bool Q4SClientSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;
    
    return ok;
}

void Q4SClientSocket::done()
{

}

void Q4SClientSocket::clear()
{
}

bool Q4SClientSocket::openConnection( int socketType )
{
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        if( socketType == SOCK_STREAM )
        {
            ok &= connectToServer( &mq4sTcpSocket, socketType );
        }
        else if( socketType == SOCK_DGRAM )
        {
            ok &= connectToServer( &mq4sUdpSocket, socketType );
        }
        else
        {
            ok &= false;
        }
    }

    return ok;
}

bool Q4SClientSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_STREAM )
    {
        ok &= mq4sTcpSocket.shutDown( );
    }
    else if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

bool Q4SClientSocket::sendTcpData( char* sendBuffer )
{
    return mq4sTcpSocket.sendData( sendBuffer );
}

bool Q4SClientSocket::receiveTcpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sTcpSocket.receiveData( receiveBuffer, receiveBufferSize );
}

bool Q4SClientSocket::sendUdpData( char* sendBuffer )
{
    return mq4sUdpSocket.sendData( sendBuffer );
}

bool Q4SClientSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize );
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool Q4SClientSocket::initializeSockets( )
{
    WSADATA     wsaData;
    int         iResult;
    bool        ok = true;

    //Initialize Winsock.

    iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if( iResult != 0 ) 
    {
        printf( "WSAStartup failed: %d\n", iResult );
        ok &= false;
    }

    return ok;
}

bool Q4SClientSocket::connectToServer( Q4SSocket* q4sSocket, int socketType )
{
    //Create a socket.
    struct addrinfo hints,
                    *ptr,
                    *pAddrInfoResult;
    int             iResult;
    bool            ok = true;
    SOCKET          socketAttempt = INVALID_SOCKET;

    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    if( socketType == SOCK_STREAM )
    {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        // Resolve the local address and port to be used by the server
        iResult = getaddrinfo( q4SClientConfigFile.serverIP.c_str(), q4SClientConfigFile.defaultTCPPort.c_str(), &hints, &pAddrInfoResult );
    }
    else if( socketType == SOCK_DGRAM )
    {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        // Resolve the local address and port to be used by the server
        iResult = getaddrinfo( q4SClientConfigFile.serverIP.c_str(), q4SClientConfigFile.defaultUDPPort.c_str(), &hints, &pAddrInfoResult );
    }
    else
    {
        ok &= false;
    }

    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        // Attempt to connect to an address until one succeeds
        for( ptr = pAddrInfoResult; ok && ( ptr != NULL ) && ( socketAttempt == INVALID_SOCKET ); ptr = ptr->ai_next ) 
        {
            // Create a SOCKET for connecting to server
            socketAttempt = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
            if( socketAttempt == INVALID_SOCKET )
            {
                printf( "socket failed with error: %ld\n", WSAGetLastError( ) );
                //WSACleanup( );
                ok &= false;
            }

            // Connect to server.
            iResult = connect( socketAttempt, ptr->ai_addr, (int)ptr->ai_addrlen );
            if( iResult == SOCKET_ERROR )
            {
                closesocket( socketAttempt );
                socketAttempt = INVALID_SOCKET;
            }
        }

        freeaddrinfo( pAddrInfoResult );

        if( socketAttempt == INVALID_SOCKET ) 
        {
            printf( "Unable to connect to server!\n" );
            //WSACleanup( );
            ok &= false;
        }
        else
        {
            q4sSocket->init( );
            if( socketType == SOCK_STREAM )
            {
                q4sSocket->setSocket( socketAttempt, socketType );
            }
            else if( socketType == SOCK_DGRAM )
            {
                q4sSocket->setSocket( socketAttempt, socketType, &q4SClientConfigFile.serverIP, &q4SClientConfigFile.defaultUDPPort );
            }
            else
            {
                ok &= false;
            }
        }
    }

    return ok;
}
