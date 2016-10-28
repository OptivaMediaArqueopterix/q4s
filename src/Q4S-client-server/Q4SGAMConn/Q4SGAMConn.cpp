// Q4SAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SGAMConnStateManager.h"
#include "Q4SGAMConnConfigFile.h"

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SGAMConnStateManager q4SGAMConnStateManager;

    ok &= q4SGAMConnStateManager.init();

    Sleep( (DWORD)q4SGAMConnConfigFile.timeEndApp);

    printf( "Saliendo de Server\n" );

    WSACleanup( );

    return 0;
}

