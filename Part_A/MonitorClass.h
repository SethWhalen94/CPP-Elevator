#ifndef __MonitorClass__
#define __MonitorClass__
#include "rt.h"

struct Status
{
	BOOL GeneralStatus;		//TRUE == Working, FALSE == BROKEN
	BOOL DoorStatus;		//TRUE == OPEN, FALSE == CLOSED
	BOOL Direction;			//TRUE == UP, FALSE == DOWN
	int Floor;

};

class Monitor
{
private:

	//Status* ptr_IO;			// a pointer to the Status struct datapool
	//Status* ptr_Dispatch;// a pointer to the Status datapool
	//CDataPool* DispatchData;	//Datapool containing the status data of the elevator for Dispatcher
	//CDataPool* IOData;			//Datapool containing the status data of the elevator for IO
	Status* ptr_status;
	CDataPool* StatusData;
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
		//DispatchData = new CDataPool(string("__DataPool__") + string(Name), sizeof(Status));
		//ptr_Dispatch = (Status*)(DispatchData->LinkDataPool());		//Link Status to datapool
		StatusData = new CDataPool(string("__DataPool__") + string(Name), sizeof(Status));

		//IOData = new CDataPool(string("__DataPool__") + string(Name), sizeof(Status));
		//ptr_IO = (Status*)(IOData->LinkDataPool());		//Link Status to datapool
		ptr_status = (Status*)(StatusData->LinkDataPool());		//Link Status to datapool

		//Semaphores
		ps1 = new CSemaphore(string("__PS1__") + string(Name), 0, 1);
		ps2 = new CSemaphore(string("__PS2__") + string(Name), 0, 1);		//=====================
		cs1 = new CSemaphore(string("__CS1__") + string(Name), 1, 1);		//Dispatcher == Consumer 1
		cs2 = new CSemaphore(string("__CS2__") + string(Name), 1, 1);		//User IO == Consumer 2
	}


//==================
// Member Functions
//================== 

	//Producer
	void Update_Status(Status status)			//Update elevators current Door status
	{
		cs1->Wait();
		cs2->Wait();
		ptr_status->DoorStatus = status.DoorStatus;
		ptr_status->Floor = status.Floor;
		ptr_status->GeneralStatus = status.GeneralStatus;
		ptr_status->Direction = status.Direction;
		ps1->Signal();
		ps2->Signal();

	}

	//Consumer #1
	Status Get_Status_Dispatch()
	{
		Status temp;
		ps1->Wait();
		temp.GeneralStatus = ptr_status->GeneralStatus;
		temp.DoorStatus = ptr_status->DoorStatus;
		temp.Floor = ptr_status->Floor;
		temp.Direction = ptr_status->Direction;
		cs1->Signal();

		return temp;

	}

	//Consumer #2
	Status Get_Status_IO()
	{
		Status temp;
		ps2->Wait();
		temp.GeneralStatus = ptr_status->GeneralStatus;
		temp.DoorStatus = ptr_status->DoorStatus;
		temp.Floor = ptr_status->Floor;
		temp.Direction = ptr_status->Direction;
		cs2->Signal();

		return temp;

	}

	//int General_Status(int service)		//Update elevators current service status
	//{
	//	cs1->Wait();
	//	cs2->Wait();
	//	ptr_status->GeneralStatus = service;
	//	ps1->Signal();
	//	ps2->Signal();

	//}

	//int Floor_Status(int floor)			//Update elevators current floor status
	//{
	//	cs1->Wait();
	//	cs2->Wait();
	//	ptr_status->Floor = floor;
	//	ps1->Signal();
	//	ps2->Signal();


	//}

	//==========================
	// Producer Member functions
	//==========================

	//This is Producer #1 for UserIO 
	//void Update_Status_Dispatch(Status status)
	//{
	//	cs1->Wait();
	//	ptr_Dispatch->DoorStatus = status.DoorStatus;
	//	ptr_Dispatch->Floor = status.Floor;
	//	ptr_Dispatch->GeneralStatus = status.GeneralStatus;
	//	ptr_Dispatch->Direction = status.Direction;
	//	ps1->Signal();

	//}

	////This is Producer #2 for UserIO 
	//void Update_Status_IO(Status status)
	//{
	//	cs2->Wait();
	//	ptr_IO->DoorStatus = status.DoorStatus;
	//	ptr_IO->Floor = status.Floor;
	//	ptr_IO->GeneralStatus = status.GeneralStatus;
	//	ptr_IO->Direction = status.Direction;
	//	ps2->Signal();

	//}
	//=================================
	// End of Producer Member functions
	//=================================


	//==========================
	// Consumer Member functions
	//==========================

	/*Status Get_Status_Dispatch()
	{
		Status temp;
		ps1->Wait();
		temp.GeneralStatus = ptr_Dispatch->GeneralStatus;
		temp.DoorStatus = ptr_Dispatch->DoorStatus;
		temp.Floor = ptr_Dispatch->Floor;
		temp.Direction = ptr_Dispatch->Direction;
		cs1->Signal();

		return temp;

	}

	Status Get_Status_IO()
	{
		Status temp;
		ps1->Wait();
		temp.GeneralStatus = ptr_IO->GeneralStatus;
		temp.DoorStatus = ptr_IO->DoorStatus;
		temp.Floor = ptr_IO->Floor;
		temp.Direction = ptr_IO->Direction;
		cs1->Signal();

		return temp;

	}*/







	//==================
	// End of Member Functions
	//==================

	//==================
	//Destructor
	//==================
	~Monitor()
	{
		
		delete StatusData;
		delete ps1;
		delete ps2;
		delete cs1;
		delete cs2;
		ptr_status = NULL;

	}
};

#endif