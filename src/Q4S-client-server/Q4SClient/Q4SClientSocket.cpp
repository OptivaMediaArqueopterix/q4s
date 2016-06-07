#include "Q4SClientSocket.h"

#define SERVER_IP "127.0.0.1"

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

bool Q4SClientSocket::connectToServer( Q4SSocket* q4sSocket )
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
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( SERVER_IP, DEFAULT_PORT, &hints, &pAddrInfoResult );
    if( iResult != 0 ) 
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
                WSACleanup( );
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
            WSACleanup( );
            ok &= false;
        }
        else
        {
            q4sSocket->init( );
            q4sSocket->setSocket( socketAttempt );
        }

    }

    return ok;
}