#include "Q4SAgentSocket.h"
#include "Q4SAgentConfigFile.h"

Q4SAgentSocket::Q4SAgentSocket ()
{
    clear();
}

Q4SAgentSocket::~Q4SAgentSocket ()
{
    done();
}

bool Q4SAgentSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;


    return ok;
}

void Q4SAgentSocket::done()
{
}

void Q4SAgentSocket::clear()
{
    mListenSocket = INVALID_SOCKET;
    mpAddrInfoResultUdp = NULL;
}

bool Q4SAgentSocket::startUdpListening( )
{
    Q4SAgentSocket     q4SAgent;
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

bool Q4SAgentSocket::stopWaiting( )
{
    bool ok = true;

    if( ok )
    {
        ok &= closeListenSocket( );
    }

    return ok;
}

bool Q4SAgentSocket::closeConnection( int socketType )
{
    bool ok = true;

    if( socketType == SOCK_DGRAM )
    {
        ok &= mq4sUdpSocket.shutDown( );
    }
    else
    {
        ok &= false;
    }

    return ok;
}

bool Q4SAgentSocket::sendUdpData( int connectionId, const char* sendBuffer )
{
    //return mq4sUdpSocket.sendData( sendBuffer );

    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;

    ok &= getConnectionInfo( connectionId, pQ4SConnInfo );
    if( ok )
    {
        ok &= mq4sUdpSocket.sendData( sendBuffer, &( pQ4SConnInfo->peerUdpAddrInfo ) );
    }

    return ok;
}

bool Q4SAgentSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize, int& connectionId )
{
    //return mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo );

    bool                ok = true;
    Q4SConnectionInfo*  pQ4SConnInfo;
    sockaddr_in         addrInfo;

    //ok &= getConnectionInfo( connectionId, pQ4SConnInfo );
    if( ok )
    {
        ok &= mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo );
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

// Functions to manage connection info list.

bool Q4SAgentSocket::getConnectionInfo( int connectionId, Q4SConnectionInfo*& pQ4SConnInfo )
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

bool Q4SAgentSocket::getConnectionInfo( sockaddr_in& connectionInfo, Q4SConnectionInfo*& pQ4SConnInfo )
{
    bool ok = false;

    std::list< Q4SConnectionInfo* >::iterator   itr_conn;

    pQ4SConnInfo = NULL;

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
    }

    if( ( pQ4SConnInfo == NULL ) )
    {
        ok = true;
    }

    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////

bool Q4SAgentSocket::initializeSockets( )
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

bool Q4SAgentSocket::startListen( )
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

bool Q4SAgentSocket::closeListenSocket( )
{
    bool ok = true;

    closesocket( mListenSocket );

    return ok;
}

bool Q4SAgentSocket::createUdpSocket( )
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
    iResult = getaddrinfo( NULL, q4SAgentConfigFile.ganyConnectorPort.c_str(), &hints, &mpAddrInfoResultUdp );
        
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

bool Q4SAgentSocket::bindUdpSocket( )
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
        senderAddr.sin_port = htons( atoi( q4SAgentConfigFile.ganyConnectorPort.c_str() ) );
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
