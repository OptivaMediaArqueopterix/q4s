// Q4SServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SCommon.h"
#include "Q4SServerSocket.h"

#include "windows.h"

DWORD WINAPI manageTcpData( LPVOID* lpParam );
DWORD WINAPI manageUdpData( LPVOID* lpParam );

int _tmain(int argc, _TCHAR* argv[])
{

    Q4SCommon           q4SCommon;
    Q4SServerSocket     q4SServer;
    bool                ok = true;
    //char                buffer[ 65536 ];
    //HANDLE              thrHandle;
    HANDLE              arrthrHandle[ 2 ];

    arrthrHandle[ 0 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageTcpData, ( void* ) &q4SServer, 0, 0 );
    arrthrHandle[ 1 ] = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )manageUdpData, ( void* ) &q4SServer, 0, 0 );
    WaitForMultipleObjects( 2, arrthrHandle, true, INFINITE );


    //if( ok )
    //{
    //    ok &= q4SServer.waitForConnections( SOCK_STREAM );
    //    //ok &= q4SServer.waitForConnections( SOCK_DGRAM );
    //}
    //if( ok )
    //{
    //    ok &= q4SServer.receiveTcpData( buffer, sizeof( buffer ) );
    //    //ok &= q4SServer.receiveUdpData( buffer, sizeof( buffer ) );
    //}
    //if( ok )
    //{
    //    ok &= q4SServer.sendTcpData( buffer );
    //    //ok &= q4SServer.sendUdpData( buffer );
    //}
    //if( ok )
    //{
    //    ok &= q4SServer.closeConnection( SOCK_STREAM );
    //    //ok &= q4SServer.closeConnection( SOCK_DGRAM );
    //}
    //if( ok )
    //{
    //    ok &= q4SServer.stopWaiting( );
    //}

    Sleep(10000);
    return ok;
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

