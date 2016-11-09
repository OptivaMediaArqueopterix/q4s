#include "Q4SServerSocket.h"
#include "Q4SServerConfigFile.h"

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
    mListenSocket   = INVALID_SOCKET;
    mUdpSocket      = INVALID_SOCKET;
    mAlertSocket    = INVALID_SOCKET;
    mpAddrInfoResultTcp     = NULL;
    mpAddrInfoResultUdp     = NULL;
    mpAddrInfoResultAlert   = NULL;
}

bool Q4SServerSocket::startTcpListening( )
{
    bool ok = true;

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

bool Q4SServerSocket::waitForTcpConnection( int connectionId )
{
    bool ok = true;

    if( ok )
    {
        Q4SConnectionInfo* connectionInfo = new Q4SConnectionInfo( );
        connectionInfo->id = connectionId;
        ZeroMemory( &( connectionInfo->peerTcpAddrInfo ), sizeof( connectionInfo->peerTcpAddrInfo ) );
        ZeroMemory( &( connectionInfo->peerUdpAddrInfo ), sizeof( connectionInfo->peerUdpAddrInfo ) );
        ok &= acceptClientConnection( connectionInfo );
    }

    return ok;
}

bool Q4SServerSocket::startUdpListening( )
{
    bool ok = true;

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
        printf( "Listening UDP at: %s\n", q4SServerConfigFile.defaultUDPPort.c_str() );
    }

    return ok;
}

bool Q4SServerSocket::stopWaiting( )
{
    bool ok = true;

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
        listConnectionInfo.front( )->q4sTcpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

bool Q4SServerSocket::sendTcpData( int connId, const char* sendBuffer )
{
    bool        ok = true;
    Q4SSocket*  pQ4SSocket;

    ok &= getTcpSocket( connId, pQ4SSocket );
    if( ok )
    {
        ok &= pQ4SSocket->sendData( sendBuffer );
    }

    return ok;
}

bool Q4SServerSocket::receiveTcpData( int connId, char* receiveBuffer, int receiveBufferSize )
{
    bool        ok = true;
    Q4SSocket*  pQ4SSocket;

    ok &= getTcpSocket( connId, pQ4SSocket );
    if( ok )
    {
        ok &= pQ4SSocket->receiveData( receiveBuffer, receiveBufferSize, NULL, q4SServerConfigFile.showSocketReceivedInfo );
    }

    return ok;
}

bool Q4SServerSocket::sendUdpData( int connectionId, const char* sendBuffer )
{
    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;

    ok &= getConnectionInfo( connectionId, pQ4SConnInfo );
    if( ok )
    {
        ok &= mq4sUdpSocket.sendData( sendBuffer, &( pQ4SConnInfo->peerUdpAddrInfo ) );
    }

    return ok;
}

bool Q4SServerSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId )
{
    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;
    sockaddr_in         addrInfo;

    if( ok )
    {
        ok &= mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo, q4SServerConfigFile.showSocketReceivedInfo );
    }
    if( ok )
    {
        ok &= getConnectionInfo( addrInfo, pQ4SConnInfo );
    }
    if( ok )
    {
        connectionId = pQ4SConnInfo->id;
        memcpy( &( pQ4SConnInfo->peerUdpAddrInfo ), &addrInfo, sizeof( addrInfo ) );
    }
    
    return ok;
}

bool Q4SServerSocket::startAlertSender()
{
    bool ok = true;

    if( ok )
    {
        ok &= initializeSockets( );
    }
    if( ok )
    {
        ok &= createAlertUdpSocket( );
    }
    if( ok )
    {
        mq4sAlertSocket.setSocket( mAlertSocket, SOCK_DGRAM, &q4SServerConfigFile.agentIP, &q4SServerConfigFile.agentPort );
        printf( "Prepared for sending alerts at: %s\n", q4SServerConfigFile.agentPort.c_str() );
    }

    return ok;
}

bool Q4SServerSocket::sendAlertData( const char* sendBuffer )
{
    bool ok = true;

    if( ok )
    {
        ok &= mq4sAlertSocket.sendData( sendBuffer);
    }

    return ok;
}


// Functions to manage connection info list.

bool Q4SServerSocket::getTcpSocket( int connectionId, Q4SSocket*& pQ4SSocket )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    pQ4SSocket = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SSocket == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( *itr_conn )->id == connectionId )
        {
            pQ4SSocket = &( ( *itr_conn )->q4sTcpSocket );
            ok = true;
        }
    }

    return ok;
}

bool Q4SServerSocket::getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    pQ4SConnInfo = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SConnInfo == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( *itr_conn )->id == connectionId )
        {
            pQ4SConnInfo = ( *itr_conn );
            ok = true;
        }
    }

    return ok;
}

bool Q4SServerSocket::getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;
    Q4SConnectionInfo*                          pQ4SConnInfoFromTcp;

    pQ4SConnInfo = NULL;
    pQ4SConnInfoFromTcp = NULL;

    for( itr_conn = listConnectionInfo.begin( ); ( pQ4SConnInfo == NULL ) && ( itr_conn != listConnectionInfo.end( ) ); itr_conn++ )
    {
        if( ( ( *itr_conn )->peerUdpAddrInfo.sin_addr.S_un.S_addr != 0 ) &&
            ( ( *itr_conn )->peerUdpAddrInfo.sin_addr.S_un.S_addr == connectionInfo.sin_addr.S_un.S_addr ) &&
            ( ( *itr_conn )->peerUdpAddrInfo.sin_port == connectionInfo.sin_port ) )
        {
            // First we try to get connection according to udp peer address information.
            pQ4SConnInfo = ( *itr_conn );
            ok = true;
        }
        else
        {
            // There's no udp address information. Perhaps it can be a new tcp connection that fits.
            if( ( pQ4SConnInfoFromTcp == NULL ) &&
                ( ( *itr_conn )->peerTcpAddrInfo.sin_addr.S_un.S_addr == connectionInfo.sin_addr.S_un.S_addr ) )
            {
                pQ4SConnInfoFromTcp = ( *itr_conn );
            }
        }
    }

    if( ( pQ4SConnInfo == NULL ) &&
        ( pQ4SConnInfoFromTcp != NULL) )
    {
        pQ4SConnInfo = pQ4SConnInfoFromTcp;
        ok = true;
    }

    return ok;
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
    iResult = getaddrinfo( NULL, q4SServerConfigFile.defaultTCPPort.c_str(), &hints, &mpAddrInfoResultTcp );

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

    printf( "Listening TCP at: %s\n", q4SServerConfigFile.defaultTCPPort.c_str() );

    return ok;
}

bool Q4SServerSocket::acceptClientConnection( Q4SConnectionInfo* connectionInfo )
{
    //Accept a connection from a client.
    SOCKET  attemptSocket = INVALID_SOCKET;
    bool    ok = true;
    int     addrlen;

    addrlen = sizeof( connectionInfo->peerTcpAddrInfo );
    // Accept a client socket.
    attemptSocket = accept( mListenSocket, ( SOCKADDR* )&( connectionInfo->peerTcpAddrInfo ), &addrlen );
    if( attemptSocket == INVALID_SOCKET )
    {
        int lastError = WSAGetLastError( );
        if( lastError == WSAEINTR )
        {
            printf( "accept interrupted\n" );
        }
        else
        {
            printf( "accept failed with error: %d\n",lastError );
        }
        closesocket( attemptSocket );
        //WSACleanup( );
        ok &= false;
    }
    else
    {
        connectionInfo->q4sTcpSocket.init( );
        connectionInfo->q4sTcpSocket.setSocket( attemptSocket, SOCK_STREAM );
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
    iResult = getaddrinfo( NULL, q4SServerConfigFile.defaultUDPPort.c_str(), &hints, &mpAddrInfoResultUdp );
        
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
        senderAddr.sin_port = htons( atoi( q4SServerConfigFile.defaultUDPPort.c_str() ) );
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

bool Q4SServerSocket::createAlertUdpSocket( )
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
    iResult = getaddrinfo( NULL, q4SServerConfigFile.agentPort.c_str(), &hints, &mpAddrInfoResultAlert );
        
    if( ok && ( iResult != 0 ) )
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        mAlertSocket = socket( mpAddrInfoResultAlert->ai_family, mpAddrInfoResultAlert->ai_socktype, mpAddrInfoResultAlert->ai_protocol );
        if( mAlertSocket == INVALID_SOCKET ) 
        {
            printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
            freeaddrinfo( mpAddrInfoResultAlert );
            WSACleanup( );
            ok &= false;
        }
    }

    return ok;
}