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
    bool                ok = true;
    char                buffer[ 65536 ];

    if( ok )
    {
        ok &= q4SServer.waitForConnections( SOCK_STREAM );
        //ok &= q4SServer.waitForConnections( SOCK_DGRAM );
    }
    if( ok )
    {
        ok &= q4SServer.receiveTcpData( buffer, sizeof( buffer ) );
        //ok &= q4SServer.receiveUdpData( buffer, sizeof( buffer ) );
    }
    if( ok )
    {
        ok &= q4SServer.sendTcpData( buffer );
        //ok &= q4SServer.sendUdpData( buffer );
    }
    if( ok )
    {
        ok &= q4SServer.closeConnection( SOCK_STREAM );
        //ok &= q4SServer.closeConnection( SOCK_DGRAM );
    }
    if( ok )
    {
        ok &= q4SServer.stopWaiting( );
    }

    Sleep(10000);
    return ok;
}

