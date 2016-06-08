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
    char                buffer[ 256 ];

    if( ok )
    {
        ok &= q4SServer.waitForConnections( );
    }
    if( ok )
    {
        ok &= q4SServer.receiveData( buffer, sizeof( buffer ) );
    }
    if( ok )
    {
        ok &= q4SServer.sendData( buffer );
    }
    if( ok )
    {
        ok &= q4SServer.closeConnection( );
    }
    if( ok )
    {
        ok &= q4SServer.stopWaiting( );
    }

    Sleep(10000);
    return ok;
}

