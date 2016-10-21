// Q4SClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SClientStateManager.h"


#include "Q4SCommon.h"

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <Windows.h>

#include "Q4SClientConfigFile.h"

#pragma comment(lib, "Ws2_32.lib")

//Q4SClientConfigFile configFile("Config.txt");

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    {
        //std::cout << "SERVER_IP: " << configFile.serverIP << "\n";
        //std::cout << "defaultTCPPort: " << configFile.defaultTCPPort << "\n";
    }

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();

    Sleep (TIME_END_APP);

    printf( "Saliendo de Client\n" );

    WSACleanup( );

    return 0;
}