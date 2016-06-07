#include "Q4SClientSocket.h"

//#define DEFAULT_BUFLEN 512
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
    //mClientSocket = INVALID_SOCKET;
    //mpAddrInfoResult = NULL;
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


//////////////////////////////////////////////////////////////////////////////////////////
/*
bool Q4SClientSocket::readAndSendData( )
{
    bool    ok = true;
    char    buf[ 256 ];
    int     bufLen = 0;
    int     iResult;
    std::string  strBuf;

    do
    {
        //scanf( "%s", buf );
        //std::cin >> buf;
        //std::iostream::getline( std::cin, strBuf );
        std::cin.getline( buf, sizeof buf );
        
        bufLen = (int)strlen( buf );
        if( bufLen > 0 )
        {
            printf( "\tSending: <%s>\n", buf );
            iResult = send( mClientSocket, buf, bufLen, 0 );
            if( iResult == SOCKET_ERROR )
            {
                printf( "send failed with error: %d\n", WSAGetLastError( ) );
                closesocket( mClientSocket );
                WSACleanup( );
                ok &= false;
            }
        }

    } while( bufLen > 0 );

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
*/