#include "..\rt.h"
#include "..\MonitorClass.h"

//===============================================
// Struct to hold commands from IO to Dispatcher
//
//This should contain variables for the following Commands:
//	1)		Floor up, Floor down (can be integer, 1 = up, 0 = down)
//	2)		Floor number	(can also be integer)
//	3)		Door open/close	(integer 0 or 1)
//
//=======================================================

//===================
// Global mailboxes
//===================
CMailbox IO_Mail;			// Create a message queue for this ChildIO Process


//===================================
//Global Rendevous object declaration
//===================================

CRendezvous r1Start("Threads_Start", 6);		//These Rendevous with wait for all threads to start or finish
CRendezvous r1Finish("Threads_Finish", 6);		//3 in Dispatcher and 3 in IO

CRendezvous r2Start("Process_Start", 3);		//These Rendevous with wait for all Processes to start or finish
CRendezvous r2Finish("Process_Finish", 3);		//2 elevators and IO.

//===================================
//Global Variables
//===================================
BOOL FINISHED = FALSE;		//Usd for end of simulation
CMutex m1("DrawMutex");
//========================
//	Dispatcher Threads
//========================

UINT __stdcall E1Status(void* args)
{
	//Thread rendevous, wait for ALL OTHER THREADS to start
	r1Start.Wait();

	Status elevator1_status;
	Monitor status1("E1Status");		//Instantiate Monitor class with same name as Elevator 1

	elevator1_status = status1.Get_Status_IO();		//Consume Status Data from datapool
	while (FINISHED == FALSE || elevator1_status.Floor != 0 || elevator1_status.DoorStatus != 1)
	{
		
		elevator1_status = status1.Get_Status_IO();		//Consume Status Data from datapool
		m1.Wait();
		MOVE_CURSOR(20, 9);
		cout << "Elevator 1 is on floor " << elevator1_status.Floor << '\n';
		m1.Signal();

	}
	
	//Wait for ALL OTHER THREADS to finish
	MOVE_CURSOR(0, 0);
	cout << "elevator 1 thread waiting...\n";
	r1Finish.Wait();
	MOVE_CURSOR(0, 0);
	cout << "elevator 1 thread TERMINATED...\n";
	return 0;
}

UINT __stdcall E2Status(void* args)
{
	//Thread rendevous, wait for ALL OTHER THREADS to start
	r1Start.Wait();

	Status elevator2_status;			//Create status structure to hold status data
	Monitor status2("E2Status");		//Instantiate Monitor class with same name as Elevator 2
	//CMutex m1("DrawMutex");
	elevator2_status = status2.Get_Status_IO();		//Consume status data from datapool

	while (FINISHED == FALSE || elevator2_status.Floor != 0 || elevator2_status.DoorStatus != 1)
	{
		elevator2_status = status2.Get_Status_IO();		//Consume status data from datapool
		m1.Wait();
		MOVE_CURSOR(20, 10);
		cout << "Elevator 2 is on floor " << elevator2_status.Floor << '\n';
		m1.Signal();

	}

	MOVE_CURSOR(0, 1);
	cout << "elevator 2 thread waiting...\n";
	r1Finish.Wait();
	MOVE_CURSOR(0, 1);
	cout << "elevator 2 thread TERMINATED...\n";
	return 0;
}

UINT __stdcall KeyboardData(void* args)
{
	//Thread rendevous, wait for ALL OTHER THREADS to start
	r1Start.Wait();
	//CMutex m1("DrawMutex");
	string Command;		//A string to hold user commands
	int sim_finished = 0;	//Used to check for 2 "e" in a row to end simulation
	CTypedPipe<string> CommandsPipe("comPipe1", 1024);			//IO -> Dispatcher Pipeline

	//Go up, to floor 4 NOTE: need to wait for elevator_status before
	//The user can 'get into' an elevator and choose a floor.

	MOVE_CURSOR(0, 0);
	cout << "Hello from IO Process....\n";
	
	//Command = "u0";
	while (FINISHED == FALSE) {
		m1.Wait();
		MOVE_CURSOR(0, 2);
		cout << "Please enter a command: ";
		m1.Signal();
		//getline(cin, Command);
		if (_kbhit())		//Check if a key was pressed
		{
			Command = _getch();		//Takes ONE character at a time

			if (Command == "e")
			{
				sim_finished++;
			}
			else
				sim_finished = 0;

			//MOVE_CURSOR(0, 10);
			//cout << "The count is " << sim_finished << '\n';
			CommandsPipe.Write(&Command);		//Write to pipeline
		}
		if (sim_finished == 2)
			FINISHED = TRUE;
		Sleep(50);
		
	}

	//Wait for ALL OTHER THREADS to finish
	MOVE_CURSOR(0, 3);
	cout << "Keyboard thread waiting...\n";
	r1Finish.Wait();
	MOVE_CURSOR(0, 3);
	cout << "Keyboard thread terminated...\n";
	return 0;
}
//========================
//	End of Dispatcher Threads
//========================

int main()
{
	//Wait for other Processes to start
	r2Start.Wait();

	cout << "Dispatcher creating typed pipeline to read commands...\n";
																		//Create typed pipeline for commands between
		
	CThread t1(E1Status, ACTIVE, NULL);
	CThread t2(E2Status, ACTIVE, NULL);
	CThread t3(KeyboardData, ACTIVE, NULL);

	t1.WaitForThread();
	t2.WaitForThread();
	t3.WaitForThread();

	//Wait for other processes to finish
	r2Finish.Wait();

	MOVE_CURSOR(0, 4);
	cout << "CHILD IO IS FINISHED THE SIMULATION....\n";

	return 0;		// exit child program by returning status value 0
				// Note we could also call exit(0) to achieve the same thing
}