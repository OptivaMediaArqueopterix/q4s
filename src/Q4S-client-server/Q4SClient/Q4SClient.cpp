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

#include "ConfigFile.cpp"

#pragma comment(lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    {
        ConfigFile cfg("Config.txt");

        //bool exists = cfg.keyExists("SERVER_IP");
        //std::cout << "keyName exits: " << std::boolalpha << exists << "\n";

        std::string serverIP = cfg.getValueOfKey<std::string>("SERVER_IP", "127.0.0.1");
        std::cout << "SERVER_IP: " << serverIP << "\n";
        double defaultTCPPort = cfg.getValueOfKey<double>("DEFAULT_TCP_PORT", 27015);
        std::cout << "DEFAULT_TCP_PORT: " << defaultTCPPort << "\n\n";
        double defaultTUPPort = cfg.getValueOfKey<double>("DEFAULT_UDP_PORT", 27016);
        std::cout << "DEFAULT_UDP_PORT: " << defaultTUPPort << "\n\n";

        //std::cin.get();
        //return 0;

    }

    Q4SClientStateManager q4SClientStateManager;

    ok &= q4SClientStateManager.init();

    Sleep (TIME_END_APP);

    printf( "Saliendo de Client\n" );

    WSACleanup( );

    return 0;
}