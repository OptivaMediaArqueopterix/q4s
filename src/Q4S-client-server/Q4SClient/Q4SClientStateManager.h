#ifndef _Q4SCLIENTSTATEMANAGER_H_
#define _Q4SCLIENTSTATEMANAGER_H_

#include "Q4SClientState.h"

class Q4SClientStateManager
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

#endif  // _Q4S-CLIENTSTATEMANAGER_H_