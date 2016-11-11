#include "Q4SClientStateManager.h"

#include <stdio.h>

#include "Q4SClientConfigFile.h"
#include "EKey.h"

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
                // Initialize protocol: thread and sockets
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
                // Trigger the establishment of the protocol
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
                bool measureOk = false;
                bool readyOk = Q4SClientProtocol::ready();
                if( readyOk )
                {
                    measureOk = Q4SClientProtocol::measure(q4SClientConfigFile.maxLatency, q4SClientConfigFile.maxJitter, 500, 10);

                    if (measureOk)
                    {
                        printf("Negotiation OK, launch GANY and press any key\n");
                        while(!EKey_anyKey())
                        {
                            stop = stop;
                        }
                        nextState = Q4SCLIENTSTATE_CONTINUITY;
                    }
                    else
                    {
                        // TODO: send the measure or the constrains not reached to the server in order to be informed 
                        //Alert
                        stop = true;
                    }
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }  
        break;

        case Q4SCLIENTSTATE_CONTINUITY:
            {
                bool readyOk = Q4SClientProtocol::ready();
                if( readyOk )
                {
                    printf("Hemos llegado a la continuidad\n");
                    continuity(q4SClientConfigFile.maxLatency, q4SClientConfigFile.maxJitter, 500, 10);
                    nextState = Q4SCLIENTSTATE_TERMINATION;
                }
            }
        break;

        case Q4SCLIENTSTATE_TERMINATION:
            {
                printf("Termination state\n");
                Q4SClientProtocol::end( );
                stop = true;
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