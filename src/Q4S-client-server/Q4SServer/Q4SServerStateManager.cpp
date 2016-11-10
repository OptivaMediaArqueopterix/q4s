#include "Q4SServerStateManager.h"

#include <stdio.h>
#include "Q4SServerConfigFile.h"

Q4SServerStateManager::Q4SServerStateManager()
{
    clear();
}

Q4SServerStateManager::~Q4SServerStateManager()
{
    done();
}

bool Q4SServerStateManager::init()
{
    // Prevention done call
    done();
    
    bool ok = true;

    // Init first state
    ok &= stateInit ( Q4SSERVERSTATE_INIT );

    ok &= run();

    return ok;
}

void Q4SServerStateManager::done()
{

}

//--private:-------------------------------------------------------------------------------

void Q4SServerStateManager::clear()
{
    q4SServerState = Q4SSERVERSTATE_INVALID;
    nextState =  Q4SSERVERSTATE_INVALID;
    stop = false;
}

bool Q4SServerStateManager::run()
{
    bool ok = true;

    // TODO: set out condition
    while (!stop) 
    {
        bool stateInitOk = stateInit(nextState);
        if ( stateInitOk )
        {
            q4SServerState = nextState;
        }
    }

    return ok;
}

bool Q4SServerStateManager::stateInit (Q4SServerState state)
{
    stateDone();

    bool ok = true;

    switch (state)
    {
        case Q4SSERVERSTATE_INIT:
            {
                bool initOk = Q4SServerProtocol::init();
                if (initOk)
                {
                    nextState = Q4SSERVERSTATE_HANDSHAKE;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;

        case Q4SSERVERSTATE_HANDSHAKE:
            {
                bool beginOk = Q4SServerProtocol::begin();
                if (beginOk)
                {
                    nextState = Q4SSERVERSTATE_NEGOTIATION;
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;

        case Q4SSERVERSTATE_NEGOTIATION:
            {
                bool measureOk = false;
                bool readyOk = Q4SServerProtocol::ready();
                if( readyOk )
                {
                    measureOk = Q4SServerProtocol::measure(q4SServerConfigFile.maxLatency, q4SServerConfigFile.maxJitter, 500, 10);
                    if (measureOk)
                    {
                        nextState = Q4SSERVERSTATE_CONTINUITY;
                    }
                    else
                    {
                        //Alert
                        Q4SServerProtocol::alert();
                        stop = true;
                        nextState = Q4SSERVERSTATE_TERMINATION;
                    }
                }
                else
                {
                    // TODO: launch error
                    stop = true;
                }
            }
        break;

        case Q4SSERVERSTATE_CONTINUITY:
            {
                bool readyOk = Q4SServerProtocol::ready();
                if (readyOk)
                {
                    Q4SServerProtocol::continuity(q4SServerConfigFile.maxLatency, q4SServerConfigFile.maxJitter, 500, 10);
                }
                nextState = Q4SSERVERSTATE_TERMINATION;
            }
        break;

        case Q4SSERVERSTATE_TERMINATION:
            {
                printf("Hemos llegado a la terminacion\n");
                Q4SServerProtocol::alert();
                Q4SServerProtocol::end();
                Q4SServerProtocol::done();
                stop = true;
            }
        break;

        default:
            {
                printf("Error: Wrong state for init");
            }
        break;
  }

  q4SServerState = state;

  return ok;
}

void Q4SServerStateManager::stateDone ()
{
    switch ( q4SServerState )
    {
        case Q4SSERVERSTATE_INVALID:
        break;

        case Q4SSERVERSTATE_INIT:
        break;

        case Q4SSERVERSTATE_HANDSHAKE:
        break;

        case Q4SSERVERSTATE_NEGOTIATION:
        break;

        case Q4SSERVERSTATE_CONTINUITY:
        break;

        case Q4SSERVERSTATE_TERMINATION:
        break;
    }
}