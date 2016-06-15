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
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SCLIENTSTATE_INIT );

    ok &= run();

    return ok;
}

void Q4SClientStateManager::done()
{

}

//--private:-------------------------------------------------------------------------------

void Q4SClientStateManager::clear()
{
    q4SClientState = Q4SCLIENTSTATE_INVALID;
    nextState =  Q4SCLIENTSTATE_INVALID;
    stop = false;
}

bool Q4SClientStateManager::run()
{
    bool ok = true;

    // TODO: set out condition
    while (!stop) 
    {
        bool stateInitOk = stateInit(nextState);
        if ( stateInitOk )
        {
            q4SClientState = nextState;
        }
    }

    return ok;
}

bool Q4SClientStateManager::stateInit (Q4SClientState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SCLIENTSTATE_INIT:
            {
                bool initOk = Q4SClientProtocol::init();
                if (initOk)
                {
                    nextState = Q4SCLIENTSTATE_HANDSHAKE;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;

        case Q4SCLIENTSTATE_HANDSHAKE:
            {
                bool beginOk = Q4SClientProtocol::begin();
                if (beginOk)
                {
                    nextState = Q4SCLIENTSTATE_NEGOTIATION;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;

        case Q4SCLIENTSTATE_NEGOTIATION:
            {
                printf("Hemos llegado a la negociación");
                stop = true;
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