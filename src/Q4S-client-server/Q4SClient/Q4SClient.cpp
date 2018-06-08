// Q4SClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SClientStateManager.h"
#include "Q4SClientConfigFile.h"

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();

    if (ok)
    {
        bool runOk = q4SClientStateManager.run();

    }
    Sleep ( (DWORD)q4SClientConfigFile.timeEndApp);

    printf( "Saliendo de Client\n" );

    WSACleanup( );

    system("pause");

    return 0;
}