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
	bool	init();
	void	done();

private:

	void	clear();

	// State

    bool	stateInit   ( Q4SClientState state );
    void    stateDone   ( );

	Q4SClientState q4SClientState;
};

#endif  // _Q4SCLIENTSTATEMANAGER_H_