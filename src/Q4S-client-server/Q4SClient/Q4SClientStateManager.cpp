#include "Q4SClientStateManager.h"

#include <stdio.h>

Q4SClientStateManager::Q4SClientStateManager()
{
	clear();
}

Q4SClientStateManager::~Q4SClientStateManager()
{
	done();
}

bool Q4SClientStateManager::init()
{
	bool ok = true;

	ok &= stateInit ( Q4SCLIENTSTATE_INIT );

	return ok;
}

void Q4SClientStateManager::done()
{

}

//--private:-------------------------------------------------------------------------------

void Q4SClientStateManager::clear()
{
	q4SClientState = Q4SCLIENTSTATE_INVALID;
}


bool Q4SClientStateManager::stateInit (Q4SClientState state)
{
	stateDone();

	bool ok = true;

	switch (state)
	{
		case Q4SCLIENTSTATE_INIT:
			{

			}
  		break;

		case Q4SCLIENTSTATE_HANDSHAKE:
			{

			}
  		break;

		case Q4SCLIENTSTATE_NEGOTIATION:
			{

			}
  		break;

		case Q4SCLIENTSTATE_CONTINUITY:
			{

			}
  		break;

		case Q4SCLIENTSTATE_TERMINATION:
			{

			}
  		break;

		default:
			{
				printf("Error: Wrong state for init");
			}
  		break;
  }

  q4SClientState = state;

  return ok;
}

void Q4SClientStateManager::stateDone ()
{
	switch ( q4SClientState )
	{
		case Q4SCLIENTSTATE_INVALID:
  		break;

  		case Q4SCLIENTSTATE_INIT:
  		break;

		case Q4SCLIENTSTATE_HANDSHAKE:
		break;

		case Q4SCLIENTSTATE_NEGOTIATION:
		break;

		case Q4SCLIENTSTATE_CONTINUITY:
		break;

		case Q4SCLIENTSTATE_TERMINATION:
  		break;
	}
}