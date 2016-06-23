// Q4SServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SServerStateManager.h"

//#include "Q4SCommon.h"
//#include "Q4SServerSocket.h"

#include "windows.h"

DWORD WINAPI manageTcpData( LPVOID* lpParam );
DWORD WINAPI manageUdpData( LPVOID* lpParam );

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SServerStateManager q4SServerStateManager;

    ok &= q4SServerStateManager.init();

    Sleep( TIME_END_APP );

    return 0;
}

DWORD WINAPI manageTcpData( LPVOID* lpParam )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    Q4SServerSocket*    pQ4sServer;

    pQ4sServer = ( Q4SServerSocket* )lpParam;
    if( ok )
    {
        ok &= pQ4sServer->waitForConnections( SOCK_STREAM );
    }
    while (ok ) 
    {
        if( ok )
        {
            ok &= pQ4sServer->receiveTcpData( buffer, sizeof( buffer ) );
        }
        if( ok )
        {
            ok &= pQ4sServer->sendTcpData( buffer );
        }
    }
    if( ok )
    {
        ok &= pQ4sServer->closeConnection( SOCK_STREAM );
    }
    if( ok )
    {
        ok &= pQ4sServer->stopWaiting( );
    }

    return ok;
}

DWORD WINAPI manageUdpData( LPVOID* lpParam )
{
    bool                ok = true;
    char                buffer[ 65536 ];
    Q4SServerSocket*    pQ4sServer;

    pQ4sServer = ( Q4SServerSocket* )lpParam;
    if( ok )
    {
        ok &= pQ4sServer->waitForConnections( SOCK_DGRAM );
    }
    if( ok )
    {
        ok &= pQ4sServer->receiveUdpData( buffer, sizeof( buffer ) );
    }
    if( ok )
    {
        ok &= pQ4sServer->sendUdpData( buffer );
    }
    if( ok )
    {
        ok &= pQ4sServer->closeConnection( SOCK_DGRAM );
    }
    //if( ok )
    //{
    //    ok &= pQ4sServer->stopWaiting( );
    //}

    return ok;

}

