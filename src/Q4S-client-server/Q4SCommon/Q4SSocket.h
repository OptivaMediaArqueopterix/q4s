#ifndef _Q4SSOCKET_H_
#define _Q4SSOCKET_H_

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>

class Q4SSocket
{
public:

    // Constructor-Destructor
    Q4SSocket( );
    ~Q4SSocket( );

    // Init-Done
    bool    init( );
    void    done( );

    void    setSocket( SOCKET socket, int socketType, std::string* connectToIP = NULL, std::string* connectToUDPPort = NULL);
    bool    sendData( const char* sendBuffer, sockaddr_in* pAddrInfo = NULL, bool showInfo = false );

    bool    receiveData( char* receiveBuffer, int receiveBufferSize, sockaddr_in* pAddrInfo, bool showInfo = false );
    bool    shutDown( );
    bool    disconnect( );

private:

    void    clear( );

    SOCKET          mSocket;
    int             mSocketType;
    std::string*    mSocketUDPConnectToIP;
    std::string*    mSocketUDPConnectToPort;

    sockaddr_in     mPeerAddrInfo;
    int             mPeerAddrInfoLen;

    bool            mAlreadyReceived;
};

#endif  // _Q4SSOCKET_H_