// Q4SAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SAgentStateManager.h"
#include "Q4SAgentConfigFile.h"

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SAgentStateManager q4SAgentStateManager;

    ok &= q4SAgentStateManager.init();

    Sleep( (DWORD)q4SAgentConfigFile.timeEndApp);

    printf( "Saliendo de Server\n" );

    WSACleanup( );

    return 0;
}

