#include "Q4SSocket.h"

#pragma comment(lib, "Ws2_32.lib")

Q4SSocket::Q4SSocket( )
{
    clear();
}

Q4SSocket::~Q4SSocket( )
{
    done();
}

bool Q4SSocket::init( )
{
    // Prevention done call
    done();

    bool ok = true;
    
    return ok;
}

void Q4SSocket::done( )
{
}

void Q4SSocket::clear( )
{
    mSocket = INVALID_SOCKET;
    mSocketType = 0;
    mPeerAddrInfoLen = 0;
    mAlreadyReceived = false;
}

void Q4SSocket::setSocket( SOCKET socket, int socketType, std::string* connectToIP, std::string* connectToUDPPort)
{
    mSocket = socket;
    mSocketType = socketType;
    mSocketUDPConnectToIP = connectToIP;
    mSocketUDPConnectToPort = connectToUDPPort;
}

bool Q4SSocket::sendData( const char* sendBuffer, sockaddr_in* pAddrInfo )
{
    //Bind the socket.
    int     iResult;
    bool    ok = true;

    // Send the buffer
    if( mSocketType == SOCK_STREAM )
    {
        iResult = send( mSocket, sendBuffer, (int)strlen( sendBuffer ), 0 );
    }
    else if( mSocketType == SOCK_DGRAM )
    {
        SOCKADDR*   addrInfoToUse;
        int         addrInfoLenToUse;
        // In client connections, we send prior to receive, we haven't peer info. We define it.
        if( pAddrInfo != NULL )
        {
            addrInfoToUse = ( SOCKADDR* )pAddrInfo;
            addrInfoLenToUse = sizeof( *pAddrInfo );
        }
        else 
        {
            if( mAlreadyReceived == false )
            {
                mPeerAddrInfo.sin_family = AF_INET;
                mPeerAddrInfo.sin_port = htons( atoi( mSocketUDPConnectToPort->c_str() ) );
                mPeerAddrInfo.sin_addr.s_addr = inet_addr( mSocketUDPConnectToIP->c_str() );
                mPeerAddrInfoLen = sizeof( mPeerAddrInfo );
            }
            addrInfoToUse = ( SOCKADDR* )&mPeerAddrInfo;
            addrInfoLenToUse = mPeerAddrInfoLen;
        }
        iResult = sendto( mSocket, sendBuffer, (int)strlen( sendBuffer ), 0, addrInfoToUse, addrInfoLenToUse );
    }
    else
    {
        ok &= false;
    }

    if( iResult == SOCKET_ERROR )
    {
        printf( "send failed with error: %d\n", WSAGetLastError( ) );
        disconnect( );
        ok &= false;
    }

    printf( "Bytes Sent: %ld\n", iResult );

    return ok;
}

bool Q4SSocket::receiveData( char* receiveBuffer, int receiveBufferSize, sockaddr_in* pAddrInfo )
{
    //Listen on the socket for a client.
    bool    ok = true;
    int     iResult;


    if( mSocketType == SOCK_STREAM )
    {
    }
    else if( mSocketType == SOCK_DGRAM )
    {
    }
    else
    {
        ok &= false;
    }
    if( mPeerAddrInfoLen == 0 )
    {
        mPeerAddrInfoLen = sizeof( mPeerAddrInfo );
    }
    iResult = recvfrom( mSocket, receiveBuffer, receiveBufferSize, 0, ( SOCKADDR* )&mPeerAddrInfo, &mPeerAddrInfoLen );
    if( pAddrInfo != NULL )
    {
        memcpy( pAddrInfo, &mPeerAddrInfo, sizeof( mPeerAddrInfo ) );
    }
    printf( "Exiting from receive in %d type\n", mSocketType );
    if( iResult > 0 )
    {
        if( mSocketType == SOCK_STREAM )
        {
            printf( "Bytes received by Tcp: %d\n", iResult );
        }
        else if( mSocketType == SOCK_DGRAM )
        {
            printf( "Bytes received by Udp: %d\n", iResult );
        }
        receiveBuffer[ iResult ] = '\0';
        mAlreadyReceived = true;
    }
    else if( iResult == 0 )
    {
        printf( "Connection closed\n" );
        ok &= false;
    }
    else
    {
        ok &= false;
        int lastError = WSAGetLastError( );
        if( lastError == WSAEINTR )
        {
            printf( "%s recv interrupted\n", ( mSocketType == 1 ? "TCP" : ( mSocketType == 2 ? "UDP" : "UNK" ) ) );
        }
        else
        {
            printf( "recv failed with error: %d\n",lastError );
        }
    }

    return ok;
}

bool Q4SSocket::shutDown( )
{
    int     iResult;
    bool    ok = true;

    // shutdown the connection since no more data will be sent
    if( mSocket != INVALID_SOCKET )
    {
        printf( "Shutting down socket...\n" );
        iResult = shutdown( mSocket, SD_SEND );
        if( iResult == SOCKET_ERROR )
        {
            printf( "shutdown failed with error: %d\n", WSAGetLastError( ) );
            ok &= false;
        }
        disconnect( );
    }

    return ok;
}

bool Q4SSocket::disconnect( )
{
    bool    ok = true;

    // cleanup
    closesocket( mSocket );

    return ok;
}