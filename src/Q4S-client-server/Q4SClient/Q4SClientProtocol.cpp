#include "Q4SClientProtocol.h"

#include "Q4SClientSocket.h"

#include <stdio.h>

Q4SClientProtocol::Q4SClientProtocol ()
{
	clear();
}

Q4SClientProtocol::~Q4SClientProtocol ()
{
	done();
}

bool Q4SClientProtocol::init()
{
	// Prevention done call
	done();

	bool ok = true;


	return ok;
}

void Q4SClientProtocol::done()
{
}

void Q4SClientProtocol::begin()
{
	printf("METHOD: begin\n");
	
	Q4SClientSocket		clientSocket;
	bool				ok = true;

	if( ok )
	{
		ok &= clientSocket.initializeSockets( );
	}
	if( ok )
	{
		ok &= clientSocket.connectToServer( );
	}
	if( ok )
	{
		ok &= clientSocket.sendData( );
	}
	if( ok )
	{
		ok &= clientSocket.receiveData( );
	}
	if( ok )
	{
		ok &= clientSocket.disconnect( );
	}
}

void Q4SClientProtocol::ready()
{
	printf("METHOD: ready\n");
}

void Q4SClientProtocol::ping()
{
	printf("METHOD: ping\n");
}

void Q4SClientProtocol::bwidth()
{
	printf("METHOD: bwidth\n");
}

void Q4SClientProtocol::cancel()
{
	printf("METHOD: cancel\n");
}

void Q4SClientProtocol::alert()
{
	printf("METHOD: alert\n");
}


void Q4SClientProtocol::clear()
{
}

