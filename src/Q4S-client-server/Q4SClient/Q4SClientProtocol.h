#ifndef _Q4SCLIENTPROTOCOL_H_
#define _Q4SCLIENTPROTOCOL_H_

class Q4SClientProtocol
{
public:

	// Constructor-Destructor
	Q4SClientProtocol ();
	~Q4SClientProtocol ();

    // Init-Done
	bool	init();
	void	done();

	// Q4S Methods
	void	begin();
	void	ready();
	void	ping();
	void	bwidth();
	void	cancel();
	void	alert();

private:

	void	clear();


};

#endif  // _Q4SCLIENTPROTOCOL_H_