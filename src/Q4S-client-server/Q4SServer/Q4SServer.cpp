// Q4SServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Q4SServerStateManager.h"

#include "windows.h"

int _tmain(int argc, _TCHAR* argv[])
{
    bool ok = true;

    Q4SServerStateManager q4SServerStateManager;

    ok &= q4SServerStateManager.init();

    Sleep( TIME_END_APP );

    return 0;
}

