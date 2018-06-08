// Q4SAgent2.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>



using namespace std;

#define VIRTUAL 0
#define DEBUG 0


#include "stdafx.h"
#include "targetver.h"
#include "Q4SAgent2.h"


#include "Q4SAgentStateManager.h"
#include "Q4SAgentConfigFile.h"



int _tmain(int argc, _TCHAR* argv[])
{
	
	bool ok = true;
	Q4SAgentStateManager q4SAgentStateManager;
	actuator.ReadConfigFile();
	//actuator.Print();

	ok &= q4SAgentStateManager.init();
	
	

	Sleep( (DWORD)q4SAgentConfigFile.timeEndApp);

    printf( "Saliendo de Server\n" );

    WSACleanup( );
	return 0;
}

