#ifndef __Monitor__
#define __Monitor__

#include "..\rt.h"

struct Status
{
	int DoorStatus;
	int GeneralStatus;
	int Floor;

};

class Monitor {

private:

	Status* ptr_status;		// a pointer to  status struct datapool
	CDataPool* theDataPool;		// a datapool containing the data to be protected double balance; 
	CMutex* theMutex;			// a pointer to a hidden mutex protecting the x variable above
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
		theMutex = new CMutex(string("__Mutex__") + string(Name));
		theDataPool = new CDataPool(string("__DataPool__") + string(Name), sizeof(Status));
		ps1 = new CSemaphore(string("__PS1__") + string(Name), 0, 1);
		ps2 = new CSemaphore(string("__PS2__") + string(Name), 0, 1);
		cs1 = new CSemaphore(string("__CS1__") + string(Name), 1, 1);
		cs2 = new CSemaphore(string("__CS2__") + string(Name), 1, 1);
		ptr_status = (Status*)(theDataPool->LinkDataPool());		//Link x to datapool

		////Semaphores for Monitor
		//CSemaphore ps1("PS1", 0, 1);
		//CSemaphore ps2("PS2", 0, 1);
		//CSemaphore cs1("CS1", 1, 1);
		//CSemaphore cs2("CS2", 1, 1);
	}

	//==================
	// Member Functions
	//==================
	int Door_Status(int door)			//Update elevators current Door status
	{
		cs1->Wait();
		cs2->Wait();
		ptr_status->DoorStatus = door;
		ps1->Signal();
		ps2->Signal();

	}

	int General_Status(int service)		//Update elevators current service status
	{
		cs1->Wait();
		cs2->Wait();
		ptr_status->GeneralStatus = service;
		ps1->Signal();
		ps2->Signal();

	}

	int Floor_Status(int floor)			//Update elevators current floor status
	{
		cs1->Wait();
		cs2->Wait();
		ptr_status->Floor = floor;
		ps1->Signal();
		ps2->Signal();


	}



	//==================
	// End of Member Functions
	//==================

	//==================
	//Destructor
	//==================
	~Monitor()
	{
		delete theMutex;
		delete theDataPool;
		delete ps1;
		delete ps2;
		delete cs1;
		delete cs2;
		ptr_status = NULL;

	}
};



#endif // __Monitor__


