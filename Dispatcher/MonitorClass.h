#ifndef __MonitorClass__
#define __MonitorClass__
#include "..\rt.h"

struct Status
{
	int DoorStatus;
	int GeneralStatus;
	int Floor;

};


class Monitor
{
private:

	Status* ptr_status;			// a pointer to the status struct datapool
	CDataPool* statusDataPool;	//Datapool containing the status data of the elevator
	CSemaphore* ps1;			// a pointer to Producer #1
	CSemaphore* ps2;			// a pointer to Producer #2
	CSemaphore* cs1;			// a pointer to Consumer #1
	CSemaphore* cs2;			// a pointer to Consumer #2

public:

//==================
//Constructor
//==================
	Monitor(string Name)
	{
		statusDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(Status));
		ps1 = new CSemaphore(string("__PS1__") + string(Name), 0, 1);
		ps2 = new CSemaphore(string("__PS2__") + string(Name), 0, 1);
		cs1 = new CSemaphore(string("__CS1__") + string(Name), 1, 1);
		cs2 = new CSemaphore(string("__CS2__") + string(Name), 1, 1);

	}



};


#endif