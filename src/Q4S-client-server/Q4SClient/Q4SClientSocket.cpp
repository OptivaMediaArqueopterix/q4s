#include "Q4SClientSocket.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
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
    mClientSocket = INVALID_SOCKET;
    mpAddrInfoResult = NULL;
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

bool Q4SClientSocket::connectToServer( )
{
    //Create a socket.
    struct addrinfo hints,
                    *ptr;
    int             iResult;
    bool            ok = true;

    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo( SERVER_IP, DEFAULT_PORT, &hints, &mpAddrInfoResult );
    if( iResult != 0 ) 
    {
        printf( "getaddrinfo failed: %d\n", iResult );
        WSACleanup( );
        ok &= false;
    }

    if( ok )
    {
        // Attempt to connect to an address until one succeeds
        for( ptr = mpAddrInfoResult; ok && ( ptr != NULL ) && ( mClientSocket == INVALID_SOCKET ); ptr = ptr->ai_next ) 
        {
            // Create a SOCKET for connecting to server
            mClientSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
            if( mClientSocket == INVALID_SOCKET )
            {
                printf( "socket failed with error: %ld\n", WSAGetLastError( ) );
                WSACleanup( );
                ok &= false;
            }

            // Connect to server.
            iResult = connect( mClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
            if( iResult == SOCKET_ERROR )
            {
                closesocket( mClientSocket );
                mClientSocket = INVALID_SOCKET;
            }
        }

        freeaddrinfo( mpAddrInfoResult );

        if( mClientSocket == INVALID_SOCKET ) 
        {
            printf( "Unable to connect to server!\n" );
            WSACleanup( );
            ok &= false;
        }
    }

    return ok;
}

bool Q4SClientSocket::sendData( )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;
    char*   sendbuf = "this is a test";

    // Send an initial buffer
    iResult = send( mClientSocket, sendbuf, (int)strlen( sendbuf ), 0 );
    if( iResult == SOCKET_ERROR )
    {
        printf( "send failed with error: %d\n", WSAGetLastError( ) );
        closesocket( mClientSocket );
        WSACleanup( );
        ok &= false;
    }

    printf( "Bytes Sent: %ld\n", iResult );

    // shutdown the connection since no more data will be sent
    iResult = shutdown( mClientSocket, SD_SEND );
    if( iResult == SOCKET_ERROR )
    {
        printf( "shutdown failed with error: %d\n", WSAGetLastError( ) );
        closesocket( mClientSocket );
        WSACleanup( );
        ok &= false;
    }

    return ok;
}

bool Q4SClientSocket::receiveData( )
{
    //Listen on the socket for a client.
    bool    ok = true;
    char    recvbuf[DEFAULT_BUFLEN];
    int     iResult;
    int     recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer closes the connection
    do 
    {
        iResult = recv( mClientSocket, recvbuf, recvbuflen, 0 );
        if( iResult > 0 )
        {
            printf( "Bytes received: %d\n", iResult );
        }
        else if( iResult == 0 )
        {
            printf("Connection closed\n");
        }
        else
        {
            printf( "recv failed with error: %d\n", WSAGetLastError( ) );
        }

    } while( iResult > 0 );

    return ok;
}

bool Q4SClientSocket::disconnect( )
{
    bool    ok = true;

    // cleanup
    closesocket( mClientSocket );
    WSACleanup( );

    return ok;
}