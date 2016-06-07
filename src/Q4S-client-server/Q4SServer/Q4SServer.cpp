// Q4SServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SCommon.h"
#include "Q4SServerSocket.h"

#include "windows.h"


int _tmain(int argc, _TCHAR* argv[])
{

    Q4SCommon           q4SCommon;
    Q4SServerSocket     q4SServer;
    Q4SSocket           q4sSocket;
    bool                ok = true;
    char                buffer[ 256 ];

    if( ok )
    {
        ok &= q4SServer.initializeSockets( );
    }
    if( ok )
    {
        ok &= q4SServer.createListenSocket( );
    }
    if( ok )
    {
        ok &= q4SServer.bindListenSocket( );
    }
    if( ok )
    {
        ok &= q4SServer.startListen( );
    }
    if( ok )
    {
        ok &= q4SServer.acceptClientConnection( &q4sSocket );
    }
    if( ok )
    {
        ok &= q4sSocket.receiveData( buffer, sizeof( buffer ) );
    }
    if( ok )
    {
        ok &= q4sSocket.sendData( buffer );
    }
    if( ok )
    {
        ok &= q4sSocket.shutDown( );
    }

    Sleep(10000);
    return 0;
}

