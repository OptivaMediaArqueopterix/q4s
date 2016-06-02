#include "Q4SServerSocket.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

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
	mClientSocket = INVALID_SOCKET;
	mpAddrInfoResult = NULL;
}

bool Q4SServerSocket::initializeSockets( )
{
	WSADATA		wsaData;
	int			iResult;
	bool		ok = true;

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
	int				iResult;
	bool			ok = true;
	
	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo( NULL, DEFAULT_PORT, &hints, &mpAddrInfoResult );
	if( iResult != 0 ) 
	{
		printf( "getaddrinfo failed: %d\n", iResult );
		WSACleanup( );
		ok &= false;
	}

	if( ok )
	{
		// Create a SOCKET for the server to listen for client connections
		mListenSocket = socket( mpAddrInfoResult->ai_family, mpAddrInfoResult->ai_socktype, mpAddrInfoResult->ai_protocol );
		if( mListenSocket == INVALID_SOCKET ) 
		{
			printf( "Error at socket(): %ld\n", WSAGetLastError( ) );
			freeaddrinfo( mpAddrInfoResult );
			WSACleanup( );
			ok &= false;
		}
	}

	return ok;
}

bool Q4SServerSocket::bindListenSocket( )
{
	//Bind the socket.
	int		iResult;
	bool	ok = true;

	// Setup the TCP listening socket
	if( mListenSocket == INVALID_SOCKET )
	{
		ok &= false;
	}
	else
	{
		iResult = bind( mListenSocket, mpAddrInfoResult->ai_addr, (int)mpAddrInfoResult->ai_addrlen );
		if( iResult == SOCKET_ERROR ) 
		{
			printf( "bind failed with error: %d\n", WSAGetLastError( ) );
			freeaddrinfo( mpAddrInfoResult );
			closesocket( mListenSocket );
			WSACleanup( );
			ok &= false;
		}
		else
		{
			freeaddrinfo( mpAddrInfoResult );
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

bool Q4SServerSocket::acceptClientConnection( )
{
    //Accept a connection from a client.
	mClientSocket = INVALID_SOCKET;
	bool ok = true;

	// Accept a client socket
	mClientSocket = accept( mListenSocket, NULL, NULL );
	if( mClientSocket == INVALID_SOCKET )
	{
		printf( "accept failed: %d\n", WSAGetLastError( ) );
		closesocket( mListenSocket );
		WSACleanup( );
		ok &= false;
	}

	return ok;
}

bool Q4SServerSocket::receiveData( )
{
	bool	ok = true;
	int		iResult;
	int		iSendResult;
	char	recvbuf[DEFAULT_BUFLEN];
    int		recvbuflen = DEFAULT_BUFLEN;

	// No longer need server socket
    closesocket(mListenSocket);

    // Receive until the peer shuts down the connection
    do 
	{
        iResult = recv( mClientSocket, recvbuf, recvbuflen, 0 );
        if( iResult > 0 )
		{
            printf( "Bytes received: %d\n", iResult );

			// Echo the buffer back to the sender
            iSendResult = send( mClientSocket, recvbuf, iResult, 0 );
            if( iSendResult == SOCKET_ERROR )
			{
                printf( "send failed with error: %d\n", WSAGetLastError( ) );
                closesocket( mClientSocket );
                WSACleanup( );
                ok &= false;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if( iResult == 0 )
		{
            printf( "Connection closing...\n" );
		}
        else 
		{
            printf( "recv failed with error: %d\n", WSAGetLastError( ) );
            closesocket( mClientSocket );
            WSACleanup( );
            ok &= false;
        }

    } while ( ( iResult > 0 ) && ( ok ) );

	return ok;
}

bool Q4SServerSocket::disconnect( )
{
	bool	ok = true;
	int		iResult;

    // shutdown the connection since we're done
    iResult = shutdown( mClientSocket, SD_SEND );
    if( iResult == SOCKET_ERROR )
	{
        printf( "shutdown failed with error: %d\n", WSAGetLastError( ) );
        closesocket( mClientSocket );
        WSACleanup( );
		ok &= false;
    }

    // cleanup
    closesocket( mClientSocket );
    WSACleanup( );

	return ok;
}