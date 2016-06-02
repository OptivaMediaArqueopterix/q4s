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

#pragma comment(lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();

    // Test for common
    Q4SCommon q4SCommon;

//    Initialize Winsock.
//    Create a socket.
//    Connect to the server.
//    Send and receive data.
//    Disconnect.

    Sleep(10000);

    return 0;
}