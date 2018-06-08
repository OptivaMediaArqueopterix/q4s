#ifndef _Q4SSERVERSTATEMANAGER_H_
#define _Q4SSERVERSTATEMANAGER_H_

#include "Q4SServerState.h"
#include "Q4SServerProtocol.h"

class Q4SServerStateManager: Q4SServerProtocol
{

public:

    // Constructor-Destructor
    Q4SServerStateManager();
    ~Q4SServerStateManager();

    // Init-Done
    bool    init();
    void    done();

    bool    run();

private:

    void    clear();

    // State
    bool    stateInit   ( Q4SServerState state );
    void    stateDone   ( );

    bool stop;
    Q4SServerState q4SServerState;
    Q4SServerState nextState;
	Q4SSDPParams mParams;    
};

#endif  // _Q4SSERVERSTATEMANAGER_H_