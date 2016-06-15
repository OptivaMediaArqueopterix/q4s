#ifndef _Q4SCLIENTSTATEMANAGER_H_
#define _Q4SCLIENTSTATEMANAGER_H_

#include "Q4SClientState.h"

#include "Q4SClientProtocol.h"

class Q4SClientStateManager : Q4SClientProtocol
{

public:

    // Constructor-Destructor
    Q4SClientStateManager();
    ~Q4SClientStateManager();

    // Init-Done
    bool    init();
    void    done();

private:

    void    clear();

    bool    run();

    // State
    bool    stateInit   ( Q4SClientState state );
    void    stateDone   ( );

    bool stop;
    Q4SClientState q4SClientState;
    Q4SClientState nextState;
    
};

#endif  // _Q4SCLIENTSTATEMANAGER_H_