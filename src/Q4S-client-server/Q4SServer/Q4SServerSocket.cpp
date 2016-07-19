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

bool Q4SServerSocket::startTcpListening( )
{
    Q4SServerSocket     q4SServer;
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createListenSocket( );
    }
    if( ok )
    {
        ok &= bindListenSocket( );
    }
    if( ok )
    {
        ok &= startListen( );
    }    

    return ok;
}

bool Q4SServerSocket::waitForTcpConnection( )
{
    //Q4SServerSocket     q4SServer;
    bool                ok = true;

    if( ok )
    {
        Q4SConnectionInfo* connectionInfo = new Q4SConnectionInfo( );
        ok &= acceptClientConnection( connectionInfo );
    }

    return ok;
}

bool Q4SServerSocket::waitForUdpConnections( )
{
    Q4SServerSocket     q4SServer;
    bool                ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createUdpSocket( );
    }
    if( ok )
    {
        ok &= bindUdpSocket( );
    }
    if( ok )
    {
        mq4sUdpSocket.setSocket( mUdpSocket, SOCK_DGRAM );
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

bool Q4SServerSocket::sendTcpData( const char* sendBuffer )
{
    //return mq4sTcpSocket.sendData( sendBuffer );
    return listConnectionInfo.front( )->mq4sTcpSocket.sendData( sendBuffer );
}

bool Q4SServerSocket::receiveTcpData( int connId, char* receiveBuffer, int receiveBufferSize )
{
    // TODO, acceed correct socket. Not kapone.
    //return mq4sTcpSocket.receiveData( receiveBuffer, receiveBufferSize );
    return listConnectionInfo.front( )->mq4sTcpSocket.receiveData( receiveBuffer, receiveBufferSize );
}

bool Q4SServerSocket::sendUdpData( const char* sendBuffer )
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

bool Q4SServerSocket::createListenSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, DEFAULT_TCP_PORT, &hints, &mpAddrInfoResultTcp );

    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        // Create a SOCKET for the server to listen for client connections
        mListenSocket = socket( mpAddrInfoResultTcp->ai_family, mpAddrInfoResultTcp->ai_socktype, mpAddrInfoResultTcp->ai_protocol );
        if( mListenSocket == INVALID_SOCKET ) 
        {
            printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
            freeaddrinfo( mpAddrInfoResultTcp );
            WSACleanup( );
            ok &= false;
        }
    }

    return ok;
}

bool Q4SServerSocket::bindListenSocket( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

    // Setup the TCP listening socket
    if( mListenSocket == INVALID_SOCKET )
    {
        ok &= false;
    }
    else
    {
        iResult = bind( mListenSocket, mpAddrInfoResultTcp->ai_addr, (int)mpAddrInfoResultTcp->ai_addrlen );
    }

    if( ok )
    {

        if( iResult == SOCKET_ERROR ) 
        {
            printf( "bind failed with error: %d\n", WSAGetLastError( ) );
            freeaddrinfo( mpAddrInfoResultTcp );
            closesocket( mListenSocket );
            WSACleanup( );
            ok &= false;
        }
        else
        {
            freeaddrinfo( mpAddrInfoResultTcp );
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

bool Q4SServerSocket::acceptClientConnection( Q4SConnectionInfo* connectionInfo )
{
    //Accept a connection from a client.
    SOCKET  attemptSocket = INVALID_SOCKET;
    bool    ok = true;
    int     addrlen;

    addrlen = sizeof( connectionInfo->peerAddrInfo );
    // Accept a client socket.
    attemptSocket = accept( mListenSocket, ( SOCKADDR* )&( connectionInfo->peerAddrInfo ), &addrlen );
    if( attemptSocket == INVALID_SOCKET )
    {
        printf( "accept failed: %d\n", WSAGetLastError( ) );
        closesocket( attemptSocket );
        //WSACleanup( );
        ok &= false;
    }
    else
    {
        connectionInfo->mq4sTcpSocket.init( );
        connectionInfo->mq4sTcpSocket.setSocket( attemptSocket, SOCK_STREAM );
        listConnectionInfo.push_back( connectionInfo );
    }

    return ok;
}

bool Q4SServerSocket::closeListenSocket( )
{
    bool ok = true;

    closesocket( mListenSocket );

    return ok;
}

bool Q4SServerSocket::createUdpSocket( )
{
    //Create a socket.
    struct addrinfo hints;
    int             iResult;
    bool            ok = true;
    
    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_INET;

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( NULL, DEFAULT_UDP_PORT, &hints, &mpAddrInfoResultUdp );
        
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        WSACleanup( );
        ok &= false;
    }

    if( ok )
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

    return ok;
}

bool Q4SServerSocket::bindUdpSocket( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

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

    if( ok )
    {
        if( iResult == SOCKET_ERROR ) 
        {
            printf( "bind failed with error: %d\n", WSAGetLastError( ) );
            freeaddrinfo( mpAddrInfoResultUdp );
            closesocket( mUdpSocket );
            WSACleanup( );
            ok &= false;
        }
        else
        {
            freeaddrinfo( mpAddrInfoResultUdp );
        }
    }

    return ok;
}
