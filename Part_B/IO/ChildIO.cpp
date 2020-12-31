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

//===================================
//Global Variables
//===================================
BOOL FINISHED = FALSE;		//Usd for end of simulation
CMutex m1("DrawMutex");
Status elevator1_status = { TRUE, TRUE, TRUE,	0 };
Status elevator2_status = { TRUE, TRUE, TRUE,	0 };

//=========================
// PASSENGER ACTIVE CLASS
//=========================

CMutex pipeM("pipeline_mutex");		//Used to make sure passengers don't write to pipeline at the same time
CTypedPipe<string> PassCommand("Passenger_Commands", 1024);			//Passenger -> IO Pipeline

class Passenger : public ActiveClass
{
private:

	int MyNumber;		//Number to keep track of each Passenger
	int CurrentFloor;
	int Destination;

	int main(void)
	{
		string com;
		string CurrentFloor_str = to_string(CurrentFloor);
		string Destination_str = to_string(Destination);
		while (1)
		{
			pipeM.Wait();
			com = PickUpOne();
			/*MOVE_CURSOR(0, 30);
			cout << "The command is " << com << '\n';*/
			PassCommand.Write(&com);//Write PickUpOne() to Dispatcher Pipeline
			//com = PickUpTwo();
			/*MOVE_CURSOR(0, 31);
			cout << "The command is " << com << '\n';*/
			PassCommand.Write(&CurrentFloor_str);//Write PickUpTwo() to Dispatcher Pipeline
			pipeM.Signal();

		/*	m1.Wait();
			MOVE_CURSOR(0, 26);
			cout << "The passenger current floor is" << CurrentFloor;
			m1.Signal();*/
			if (Destination >= CurrentFloor)		//Passenger waits for elevator 1
			{
				while (elevator1_status.Floor != CurrentFloor || elevator1_status.DoorStatus != TRUE)	//Wait for elevator to get to current floor, and open its doors
				{
					m1.Wait();
					MOVE_CURSOR(0, 27);
					cout << "The elevator is on floor " << elevator1_status.Floor;
					MOVE_CURSOR(0, 28);
					cout << "The door status is " << elevator1_status.DoorStatus;
					m1.Signal();
				}
			}
			else //Passenger waits for elevator 2
			{
				while (elevator2_status.Floor != CurrentFloor || elevator2_status.DoorStatus != TRUE)	//Wait for elevator to get to current floor, and open its doors
				{
				}
			}

			pipeM.Wait();
			com = DropOffOne();
			PassCommand.Write(&com);//Write DropOffOne() to Dispatcher Pipeline
			//com = DropOffTwo();
			PassCommand.Write(&Destination_str);//Write DropOffTwo() to Dispatcher Pipeline
			pipeM.Signal();

			if (Destination >= CurrentFloor)		//Passenger waits for elevator 1
			{
				while (elevator1_status.Floor != Destination || elevator1_status.DoorStatus != TRUE)	//Wait for elevator to get to Destination floor, and open its doors
				{
					m1.Wait();
					MOVE_CURSOR(0, 27);
					cout << "The elevator is on floor " << elevator1_status.Floor;
					MOVE_CURSOR(0, 28);
					cout << "The door status is " << elevator1_status.DoorStatus;
					m1.Signal();
				}
			}
			else //Passenger waits for elevator 2
			{
				while (elevator2_status.Floor != Destination || elevator2_status.DoorStatus != TRUE)	//Wait for elevator to get to Destination floor, and open its doors
				{
				}
			}
			//Terminate Passenger Thread
			break;

		}
		m1.Wait();
		MOVE_CURSOR(20, 15+MyNumber);
		cout << "Passenger #" << MyNumber << " Has reached their floor and terminated...\n";
		m1.Signal();
		return 0;
	}

public:
	Passenger(int number, int curr, int dest) : MyNumber(number), CurrentFloor(curr), Destination(dest)
	{

	}

private:
	string PickUpOne()		//Returns the first command for elevator pickup, Will be " u, d"
	{
		if (Destination >= CurrentFloor)
			return "u";
		else
			return "d";
	}

	string PickUpTwo()		//Returns the second command for elevator pickup, Will be " 0-9"
	{
		return to_string(MyNumber);
	}

	string DropOffOne()		//Returns the first command for elevator Drop off, Will be "1" if PickUpOne = 'u' and "2" if PickUpOne = 'd'
	{
		//while (elevator1_status.Floor != MyNumber)	//Stay here until Elevator Gets to the Pick up floor
		//{
		//}

		if (Destination >= CurrentFloor)
			return "1";
		else
			return "2";

	}

	string DropOffTwo()		//Returns the second command for elevator Drop off, Will be " 0-9"
	{

		if (MyNumber % 2 == 0)
			return "9";
		else
			return "0";

	}
};

//=========================
// End Of PASSENGER ACTIVE CLASS
//=========================

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


//========================
//	Dispatcher Threads
//========================

UINT __stdcall E1Status(void* args)
{
	//Thread rendevous, wait for ALL OTHER THREADS to start
	r1Start.Wait();

	/*Status elevator1_status;*/
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

	/*Status elevator2_status;*/			//Create status structure to hold status data
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
	int count = 0;			//Used to make sure 2 commands have been entered before exiting Keyboard input
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
			while (count < 2)
			{
				Command = _getch();		//Takes ONE character at a time

				if (Command == "e")
				{
					sim_finished++;
				}
				else
					sim_finished = 0;

				CommandsPipe.Write(&Command);		//Write to pipeline
				count++;	//Increment count
			}
			count = 0;	//Set back to zero after command is finished
		}
		else if (PassCommand.TestForData() > 0)		//Take input for Passenger objects, check that there is data in the pipeline
		{
			//Read First Command from passenger pipeline into Dispatcher pipeline
			PassCommand.Read(&Command);

			CommandsPipe.Write(&Command);

			//Read Second command from passenger pipeline into Dispatcher pipeline
			PassCommand.Read(&Command);
			CommandsPipe.Write(&Command);

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

				//	P#			Curr_floor		Destination
	Passenger p1(	1,			3,					 9);		//Creating passenger
	Passenger p2(2, 6, 0);
	Passenger p3(3, 0, 5);
	Passenger p4(4, 3, 1);
	Passenger p5(5, 1, 6);
	Passenger p6(6, 9, 9);
	Passenger p7(7, 8, 1);
	Passenger p8(8, 2, 8);
	p1.Resume();				//Resume since Active objects start in suspended state
	p2.Resume();
	p3.Resume();
	p4.Resume();
	p5.Resume();
	p6.Resume();
	p7.Resume();
	p8.Resume();

	//Wait for passenger to terminate
	p1.WaitForThread();
	p2.WaitForThread();
	p3.WaitForThread();
	p4.WaitForThread();
	p5.WaitForThread();
	p6.WaitForThread();
	p7.WaitForThread();
	p8.WaitForThread();


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