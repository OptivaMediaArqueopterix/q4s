#include "Q4SClientProtocol.h"

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
	printf("METHOD: begin");
}

void Q4SClientProtocol::ready()
{
	printf("METHOD: ready");
}

void Q4SClientProtocol::ping()
{
	printf("METHOD: ping");
}

void Q4SClientProtocol::bwidth()
{
	printf("METHOD: bwidth");
}

void Q4SClientProtocol::cancel()
{
	printf("METHOD: cancel");
}

void Q4SClientProtocol::alert()
{
	printf("METHOD: alert");
}


void Q4SClientProtocol::clear()
{
}

