#include "Q4SGAMConnStateManager.h"

#include <stdio.h>

Q4SGAMConnStateManager::Q4SGAMConnStateManager()
{
    clear();
}

Q4SGAMConnStateManager::~Q4SGAMConnStateManager()
{
    done();
}

bool Q4SGAMConnStateManager::init()
{
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SAGENTSTATE_INIT );

    ok &= run();

    return ok;
}

void Q4SGAMConnStateManager::done()
{

}

//--private:-------------------------------------------------------------------------------

void Q4SGAMConnStateManager::clear()
{
    q4SAgentState = Q4SAGENTSTATE_INVALID;
    nextState =  Q4SAGENTSTATE_INVALID;
    stop = false;
}

bool Q4SGAMConnStateManager::run()
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

bool Q4SGAMConnStateManager::stateInit (Q4SGAMConnState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SAGENTSTATE_INIT:
            {
                bool initOk = Q4SGAMConnProtocol::init();
                if (initOk)
                {
                    nextState = Q4SAGENTSTATE_LISTEN;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;
    
        case Q4SAGENTSTATE_LISTEN:
            {
                bool initOk = Q4SGAMConnProtocol::listen();
                if (initOk)
                {
                    nextState = Q4SAGENTSTATE_INVALID;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
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

void Q4SGAMConnStateManager::stateDone ()
{
    switch ( q4SAgentState )
    {
        case Q4SAGENTSTATE_INVALID:
        break;

        case Q4SAGENTSTATE_INIT:
        break;
    }
}