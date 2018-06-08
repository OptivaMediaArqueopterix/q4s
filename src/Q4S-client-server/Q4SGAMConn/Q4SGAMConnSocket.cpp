#include "Q4SGAMConnSocket.h"

#include "Q4SGAMConnConfigFile.h"

Q4SGAMConnSocket::Q4SGAMConnSocket ()
{
    clear();
}

Q4SGAMConnSocket::~Q4SGAMConnSocket ()
{
    done();
}

bool Q4SGAMConnSocket::init()
{
    // Prevention done call
    done();

    bool ok = true;


    return ok;
}

void Q4SGAMConnSocket::done()
{
}

void Q4SGAMConnSocket::clear()
{
    mpAddrInfoResultUdp = NULL;
}

bool Q4SGAMConnSocket::startUdpListening( )
{
    Q4SGAMConnSocket     q4SAgent;
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

bool Q4SGAMConnSocket::closeConnection( int socketType )
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

/*bool Q4SGAMConnSocket::sendUdpData( int connectionId, const char* sendBuffer )
{
    bool ok = true;

    if( ok )
    {
        ok &= mq4sUdpSocket.sendData( sendBuffer, &(mpAddrInfoResultUdp) );
    }

    return ok;
}*/

bool Q4SGAMConnSocket::receiveUdpData( char* receiveBuffer, int receiveBufferSize)
{
    bool                ok = true;
    sockaddr_in         addrInfo;

    if( ok )
    {
        ok &= mq4sUdpSocket.receiveData( receiveBuffer, receiveBufferSize, &addrInfo );
    }
    
    return ok;
}

/////////////////////////////////////////////////////////////////////////////////////

bool Q4SGAMConnSocket::initializeSockets( )
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

bool Q4SGAMConnSocket::createUdpSocket( )
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
    iResult = getaddrinfo( NULL, q4SGAMConnConfigFile.listenUDPPort.c_str(), &hints, &mpAddrInfoResultUdp );
        
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

bool Q4SGAMConnSocket::bindUdpSocket( )
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
        senderAddr.sin_port = htons( atoi( q4SGAMConnConfigFile.listenUDPPort.c_str() ) );
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
