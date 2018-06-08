#ifndef _Q4SGAMCONNSTATEMANAGER_H_
#define _Q4SGAMCONNSTATEMANAGER_H_

#include "Q4SGAMConnState.h"
#include "Q4SGAMConnProtocol.h"

class Q4SGAMConnStateManager : Q4SGAMConnProtocol
{

public:

    // Constructor-Destructor
    Q4SGAMConnStateManager();
    ~Q4SGAMConnStateManager();

    // Init-Done
    bool    init();
    void    done();

private:

    void    clear();

    bool    run();

    // State
    bool    stateInit   ( Q4SGAMConnState state );
    void    stateDone   ( );

    bool stop;
    Q4SGAMConnState q4SAgentState;
    Q4SGAMConnState nextState;
    
};

#endif  // _Q4SGAMCONNSTATEMANAGER_H_