#include "Q4SAgentStateManager.h"

#include <stdio.h>

Q4SAgentStateManager::Q4SAgentStateManager()
{
    clear();
}

Q4SAgentStateManager::~Q4SAgentStateManager()
{
    done();
}

bool Q4SAgentStateManager::init()
{
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SAGENTSTATE_INIT );

    ok &= run();

    return ok;
}

void Q4SAgentStateManager::done()
{

}

//--private:-------------------------------------------------------------------------------

void Q4SAgentStateManager::clear()
{
    q4SAgentState = Q4SAGENTSTATE_INVALID;
    nextState =  Q4SAGENTSTATE_INVALID;
    stop = false;
}

bool Q4SAgentStateManager::run()
{
    bool ok = true;

    // TODO: set out condition
    while (!stop) 
    {
        bool stateInitOk = stateInit(nextState);
        if ( stateInitOk )
        {
            q4SAgentState = nextState;
        }
    }

    return ok;
}

bool Q4SAgentStateManager::stateInit (Q4SAgentState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SAGENTSTATE_INIT:
            {
            }
        break;

        default:
            {
                printf("Error: Wrong state for init");
            }
        break;
  }

  q4SAgentState = state;

  return ok;
}

void Q4SAgentStateManager::stateDone ()
{
    switch ( q4SAgentState )
    {
        case Q4SAGENTSTATE_INVALID:
        break;

        case Q4SAGENTSTATE_INIT:
        break;
    }
}