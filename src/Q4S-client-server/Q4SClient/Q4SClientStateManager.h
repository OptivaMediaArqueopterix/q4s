#ifndef _Q4SCLIENTSTATEMANAGER_H_
#define _Q4SCLIENTSTATEMANAGER_H_

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
};

#endif  // _Q4S-CLIENTSTATEMANAGER_H_