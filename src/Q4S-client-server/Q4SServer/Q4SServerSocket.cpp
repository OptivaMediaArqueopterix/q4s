#include "Q4SServerSocket.h"

Q4SServerSocket::Q4SServerSocket ()
{
    clear();
}

Q4SServerSocket::~Q4SServerSocket ()
{
    done();
}

bool Q4SServerSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;


    return ok;
}

void Q4SServerSocket::done()
{
}

void Q4SServerSocket::clear()
{
    mListenSocket = INVALID_SOCKET;
    mpAddrInfoResultTcp = NULL;
    mpAddrInfoResultUdp = NULL;
}

bool Q4SServerSocket::waitForConnections( int socketType )
{
    Q4SServerSocket     q4SServer;
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createListenSocket( socketType );
    }
    if( ok )
    {
        ok &= bindListenSocket( socketType );
    }
    if( ok )
    {
        if( socketType == SOCK_DGRAM )
        {
            mq4sUdpSocket.setSocket( mUdpSocket, SOCK_DGRAM );
        }
        else if( socketType == SOCK_STREAM )
        {
            if( ok )
            {
                ok &= startListen( );
            }    
            if( ok )
            {
                ok &= acceptClientConnection( &mq4sTcpSocket );
            }
        }
        else
        {
            ok &= false;
        }
    }

    return ok;
}

bool Q4SServerSocket::stopWaiting( )
{
    bool                ok = true;

    if( ok )
    {
        ok &= closeListenSocket( );
    }

    return ok;
}

bool Q4SServerSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else if( socketType == SOCK_STREAM )
    {
        ok &= mq4sTcpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

bool Q4SServerSocket::sendTcpData( char* sendBuffer )
{
    return mq4sTcpSocket.sendData( sendBuffer );
}

bool Q4SServerSocket::receiveTcpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sTcpSocket.receiveData( receiveBuffer, receiveBufferSize );
}

bool Q4SServerSocket::sendUdpData( char* sendBuffer )
{
    return mq4sUdpSocket.sendData( sendBuffer );
}

bool Q4SServerSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize )
{
    return mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize );
}

/////////////////////////////////////////////////////////////////////////////////////

bool Q4SServerSocket::initializeSockets( )
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

bool Q4SServerSocket::createListenSocket( int socketType )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_INET;

    if( socketType == SOCK_STREAM )
    {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
        // Resolve the local address and port to be used by the server
        iResult = getaddrinfo( NULL, DEFAULT_TCP_PORT, &hints, &mpAddrInfoResultTcp );
    }
    else if( socketType == SOCK_DGRAM )
    {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        // Resolve the local address and port to be used by the server
        iResult = getaddrinfo( NULL, DEFAULT_UDP_PORT, &hints, &mpAddrInfoResultUdp );
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
        // Create a SOCKET for the server to listen for client connections
        if( socketType == SOCK_STREAM )
        {
            mListenSocket = socket( mpAddrInfoResultTcp->ai_family, mpAddrInfoResultTcp->ai_socktype, mpAddrInfoResultTcp->ai_protocol );
            if( mListenSocket == INVALID_SOCKET ) 
            {
                printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
                freeaddrinfo( mpAddrInfoResultTcp );
                WSACleanup( );
                ok &= false;
            }
        }
        else if( socketType == SOCK_DGRAM )
        {
            mUdpSocket = socket( mpAddrInfoResultUdp->ai_family, mpAddrInfoResultUdp->ai_socktype, mpAddrInfoResultUdp->ai_protocol );
            if( mUdpSocket == INVALID_SOCKET ) 
            {
                printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
                freeaddrinfo( mpAddrInfoResultUdp );
                WSACleanup( );
                ok &= false;
            }
        }
        else
        {
            ok &= false;
        }
    }

    return ok;
}

bool Q4SServerSocket::bindListenSocket( int socketType )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

    // Setup the TCP listening socket
    if( socketType == SOCK_STREAM )
    {
        if( mListenSocket == INVALID_SOCKET )
        {
            ok &= false;
        }
        else
        {
            iResult = bind( mListenSocket, mpAddrInfoResultTcp->ai_addr, (int)mpAddrInfoResultTcp->ai_addrlen );
        }
    }
    else if( socketType == SOCK_DGRAM )
    {
        if( mUdpSocket == INVALID_SOCKET )
        {
            ok &= false;
        }
        else
        {
            sockaddr_in    senderAddr;
            senderAddr.sin_family = AF_INET;
            senderAddr.sin_port = htons( atoi( DEFAULT_UDP_PORT ) );
            senderAddr.sin_addr.s_addr = htonl( INADDR_ANY ); 
            iResult = bind( mUdpSocket, ( SOCKADDR* ) &senderAddr, sizeof( senderAddr ) );
        }
    }
    else
    {
        ok &= false;
    }

    if( ok )
    {

        if( iResult == SOCKET_ERROR ) 
        {
            printf( "bind failed with error: %d\n", WSAGetLastError( ) );
            if( socketType == SOCK_STREAM )
            {
                freeaddrinfo( mpAddrInfoResultTcp );
                closesocket( mListenSocket );
            }
            else if( socketType == SOCK_DGRAM )
            {
                freeaddrinfo( mpAddrInfoResultUdp );
                closesocket( mUdpSocket );
            }
            else
            {
                ok &= false;
            }
            WSACleanup( );
            ok &= false;
        }
        else
        {
            if( socketType == SOCK_STREAM )
            {
                freeaddrinfo( mpAddrInfoResultTcp );
            }
            else if( socketType == SOCK_DGRAM )
            {
                freeaddrinfo( mpAddrInfoResultUdp );
            }
            else
            {
                ok &= false;
            }
        }
    }

    return ok;
}

bool Q4SServerSocket::startListen( )
{
    //Listen on the socket for a client.
    bool ok = true;

    if( listen( mListenSocket, SOMAXCONN ) == SOCKET_ERROR )
    {
        printf( "Listen failed with error: %ld\n", WSAGetLastError( ) );
        closesocket( mListenSocket );
        WSACleanup( );
        ok &= false;
    }

    return ok;
}

bool Q4SServerSocket::acceptClientConnection( Q4SSocket* q4sSocket )
{
    //Accept a connection from a client.
    SOCKET attemptSocket = INVALID_SOCKET;
    bool ok = true;

    // Accept a client socket.
    attemptSocket = accept( mListenSocket, NULL, NULL );
    if( attemptSocket == INVALID_SOCKET )
    {
        printf( "accept failed: %d\n", WSAGetLastError( ) );
        closesocket( attemptSocket );
        WSACleanup( );
        ok &= false;
    }
    else
    {
        q4sSocket->init( );
        q4sSocket->setSocket( attemptSocket, SOCK_STREAM );
    }

    return ok;
}

bool Q4SServerSocket::closeListenSocket( )
{
    bool ok = true;

    closesocket( mListenSocket );

    return ok;
}
