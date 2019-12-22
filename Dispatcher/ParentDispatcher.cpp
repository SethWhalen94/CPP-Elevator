#include "..\rt.h"
#include "..\MonitorClass.h"


//===============================================
// Struct to hold commands from IO to Dispatcher
//
//This should contain variables for the following Commands:
//	1)		Floor up, Floor down (BOOL, up = TRUE, down = FALSE)
//	2)		Floor number	(integer 0-9)
//	3)		Door open/close	(BOOL open = TRUE, closed = FALSE)
//
//=======================================================
//struct Status
//{
//	int DoorStatus;
//	int GeneralStatus;
//	int Floor;
//
//};

//============================
//	Global Variables
//============================
//Elevator1 thread variables
Status elevator1_status;

//Elevator 2 variables
Status elevator2_status;

//ChildIO variables
CTypedPipe<string> CommandsPipe("comPipe1", 1024);					//Create typed pipeline for commands between
string Command1;
string Command2;
BOOL Done_Commands = FALSE;
BOOL FINISHED = FALSE;		//Use for end of simulation to end all threads
CMutex draw("DrawMutex");



//============================
//End of Global Variables
//============================

//===================
// Global mailboxes
//===================
//CMailbox Elevator1_Mail;	// Create a message queue for Elevator 1 Process
//CMailbox Elevator2_Mail;	// Create a message queue for Elevator 2 Process
//CMailbox IO_Mail;			// Create a message queue for ChildIO Process

//===================================
//Global Rendevous object declaration
//===================================

CRendezvous r1Start("Threads_Start", 6);		//These Rendevous with wait for all threads to start or finish
CRendezvous r1Finish("Threads_Finish", 6);		//3 in Dispatcher and 3 in IO

CRendezvous r2Start("Process_Start", 3);		//These Rendevous with wait for all Processes to start or finish
CRendezvous r2Finish("Process_Finish", 3);		//2 elevators and IO.

CSemaphore Dispatch("Dispatcher_Mutex", 0, 1);

//========================
//	Dispatcher Threads
//========================

//string Command;

UINT __stdcall E1Status(void* args)
{

	//Wait for all other threads to start
	r1Start.Wait();
	Monitor status1("E1Status");		//Instantiate Monitor class with same name as Elevator 1
	//std::cout << "Waiting for elevator 1 status...\n";
	elevator1_status = status1.Get_Status_Dispatch();
	while (FINISHED == FALSE || elevator1_status.Floor != 0 || elevator1_status.DoorStatus != 1)
	{
		elevator1_status = status1.Get_Status_Dispatch();

		
	}

	//Wait for all other threads to finish
	MOVE_CURSOR(0, 28);
	cout << "elevator 1 thread waiting...\n";
	r1Finish.Wait();
	MOVE_CURSOR(0, 28);
	cout << "elevator 1 thread TERMINATED...\n";

	return 0;
}

UINT __stdcall E2Status(void* args)
{

	//Wait for all other threads to start
	r1Start.Wait();
	Monitor status2("E2Status");		//Instantiate Monitor class with same name as Elevator 2

	elevator2_status = status2.Get_Status_Dispatch();
	while(FINISHED == FALSE || elevator2_status.Floor != 0 || elevator2_status.DoorStatus != 1)
	{
		elevator2_status = status2.Get_Status_Dispatch();
	}

	//Wait for all other threads to finish
	MOVE_CURSOR(0, 29);
	cout << "elevator 2 thread waiting...\n";
	r1Finish.Wait();
	MOVE_CURSOR(0, 29);
	cout << "elevator 2 thread TERMINATED...\n";
	return 0;
}

UINT __stdcall IOData(void* args)
{

	//Wait for all other threads to start
	r1Start.Wait();

	MOVE_CURSOR(0, 0);
	std::cout << "Dispatcher creating typed pipeline to read commands...\n";
	
	
	//draw.Wait();
	MOVE_CURSOR(0, 1);
	std::cout << "Ready to accept Commands \n\n";
	//draw.Signal();

	while (FINISHED == FALSE)
	{
	Invalid_command_1:	//Return here if Command #1 is Invalid
		if (CommandsPipe.TestForData() >= 1)	//Check if There is data to read
		{
			Done_Commands = FALSE;		//Reset DONE COMMAND flag

			CommandsPipe.Read(&Command1);		//Read First command, Will be "1, 2, u, d, +, -, e"
			MOVE_CURSOR(0, 20);
			cout << "READ command 1 = " << Command1 << '\n';
			MOVE_CURSOR(0, 21);
			if (Command1 != "1" && Command1 != "2" && Command1 != "u" && Command1 != "d" && Command1 != "+" && Command1 != "-" && Command1 != "e")
			{
				MOVE_CURSOR(0, 2);
				std::cout << "That is an invalid command #1\n";
				goto Invalid_command_1;		//If the command is invalid, tell user to enter in another command #1

			}
			Dispatch.Signal();					//Signal the Dispatch Parent that the FIRST command is ready

			while (Done_Commands == FALSE)
			{Invalid_command_2:
				CommandsPipe.Read(&Command2);	//Read SECOND command, Will be "0-9 or e"
				MOVE_CURSOR(0, 22);
				cout << "READ command 2 = " << Command2 << '\n';

				if (Command2 != "0" && Command2 != "1" && Command2 != "2" && Command2 != "3" && Command2 != "4" && Command2 != "5" &&
					Command2 != "6" && Command2 != "7" && Command2 != "8" && Command2 != "9" && Command2 != "e")
				{
					MOVE_CURSOR(0, 2);
					std::cout << "That is an invalid command #2\n";
					goto Invalid_command_2;		//If the command is invalid, tell user to enter in another command #1

				}
				else if (Command1 != "e" & Command2 == "e")
				{
					MOVE_CURSOR(0, 2);
					std::cout << "That is an invalid command #2\n";
					goto Invalid_command_2;		//If the command is invalid, tell user to enter in another command #1
				}

				Dispatch.Signal();				//Signal Parent that SECOND command is ready to be read
				Done_Commands = TRUE;
			}

			//draw.Signal();	//Done using screen
		}
	}

	//cout << "DONE SIMULATION... \n";

	//Wait for all other threads to finish
	MOVE_CURSOR(0, 30);
	cout << "IO thread waiting...\n";
	r1Finish.Wait();

	MOVE_CURSOR(0, 30);
	cout << "IO thread TERMINATED...\n";
	return 0;
}
//========================
//	End of Dispatcher Threads
//========================

//============================================================
// Global declaration of objects/variables used by Dispatcher
//============================================================
//Commands costatus1;													//Dispatcher and IO
//======================
// End of global items
//======================

int main()
{
	BOOL Invalid_command = FALSE;		//Keeps track of invalid command
	//=====================
	//	Creating Threads
	//=====================
	CThread t1(IOData, ACTIVE, NULL);
	CThread t2(E1Status, ACTIVE, NULL);
	CThread t3(E2Status, ACTIVE, NULL);


	//=====================
	//	Creating Processes
	//=====================
	CProcess UserIO("E:\\Panda\\Desktop\\CPEN_333_Projects\\Assignment_1_PartA\\Assignment_1\\Debug\\IO.exe",
					NORMAL_PRIORITY_CLASS,			// priority
					OWN_WINDOW,						// process has its own window					
					ACTIVE							// process is active immediately
	);

	CProcess Elevator1("E:\\Panda\\Desktop\\CPEN_333_Projects\\Assignment_1_PartA\\Assignment_1\\Debug\\Elevator_1.exe",
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	CProcess Elevator2("E:\\Panda\\Desktop\\CPEN_333_Projects\\Assignment_1_PartA\\Assignment_1\\Debug\\Elevator_2.exe",
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
	);

	SLEEP(1000);
	//std::cout << "Child Processes Activated.....\n";

	//========================
	//Logic of Dispatcher
	//===========================
	while (FINISHED == FALSE)
	{
		MOVE_CURSOR(0, 5);
		std::cout << "Waiting for Command #1...\n";
		Dispatch.Wait();							//Read First command, Will be "1, 2, u, d, +, -, e"
		MOVE_CURSOR(0, 6);
		std::cout << "Got command " << Command1 << '\n';

		MOVE_CURSOR(0, 7);
		std::cout << "Waiting for Command #2...\n";
		Dispatch.Wait();							//Read Second command, Will be "0-9, or e"
		MOVE_CURSOR(0, 8);
		std::cout << "Got command " << Command2 << '\n';
		

		MOVE_CURSOR(0, 9);
		if (Command2 == "0")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(110);		//INPUT : 10
					std::cout << "Sent Mail 110 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(210);		//INPUT : 20	
					std::cout << "Sent Mail 210 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(100);		//INPUT : u0
					std::cout << "Sent Mail 100 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				//This means someone is on Floor '0' requesting to go down, this doesn't make sense, ignore this request
				std::cout << "You are already on the lowest floor...\n";
			}

		}

		//=== COMMAND 2 IS ONE ====//
		else if (Command2 == "1")		
		{
			//Check if first command was "+, -, 1, 2, u, or d"
			if (Command1 == "1")
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(111);
					std::cout << "Sent Mail 111 ..\n";
				}
			}
			else if (Command1 == "2")
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(211);
					std::cout << "Sent Mail 211 ..\n";
				}

			}

			else if (Command1 == "+")
			{
				if (elevator1_status.GeneralStatus == FALSE)
				{
					//Put elevator 1 back in service
					Elevator1.Post(500);
					std::cout << "Sent Mail 500 to Elevator 1 ..\n";
				}
				else
					std::cout << "Elevator 1 already IN SERVICE...\n";
			}

			else if (Command1 == "-")
			{
				if (elevator1_status.GeneralStatus == TRUE)
				{
					//Put elevator 1 OUT OF SERVICE
					Elevator1.Post(501);
					std::cout << "Sent Mail 501 to Elevator 1 ..\n";
				}
				else
					std::cout << "Elevator 1 already OUT OF SERVICE...\n";
			}

			else if (Command1 == "u")
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(101);
					std::cout << "Sent Mail 101 ..\n";
				}
			}

			else if (Command1 == "d")
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(201);
					std::cout << "Sent Mail 201 ..\n";
				}
			}

		}

		//=== COMMAND 2 IS TWO ====//
		else if (Command2 == "2")
		{
			//Check if first command was "+, -, 1, 2, u, or d"
			if (Command1 == "1")
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(112);
					std::cout << "Sent Mail 112 ..\n";
				}
			}
			else if (Command1 == "2")
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(212);
					std::cout << "Sent Mail 212 ..\n";
				}

			}

			else if (Command1 == "+")
			{
				if (elevator2_status.GeneralStatus == FALSE)
				{
					//Put elevator 2 back in service
					Elevator2.Post(500);
					std::cout << "Sent Mail 500 to Elevator 2 ..\n";
				}
				else
					std::cout << "Elevator 2 already in Service...\n";
			}

			else if (Command1 == "-")
			{
				if (elevator2_status.GeneralStatus == TRUE)
				{
					//Put elevator 2 OUT OF SERVICE
					Elevator2.Post(501);
					std::cout << "Sent Mail 501 to elevator 2 ..\n";
				}
				else
					std::cout << "Elevator 2 already OUT OF SERVICE...\n";
			}

			else if (Command1 == "u")
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(102);
					std::cout << "Sent Mail 102 ..\n";
				}
			}

			else if (Command1 == "d")
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(202);
					std::cout << "Sent Mail 202 ..\n";
				}
			}
		}

		//=== COMMAND 2 IS THREE ====//
		else if (Command2 == "3")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(113);
					std::cout << "Sent Mail 113 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(213);
					std::cout << "Sent Mail 213 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(103);
					std::cout << "Sent Mail 103 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(203);
					std::cout << "Sent Mail 203 ..\n";
				}
			}
		}

		//=== COMMAND 2 IS FOUR ====//
		else if (Command2 == "4")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(114);
					std::cout << "Sent Mail 114 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(214);
					std::cout << "Sent Mail 214 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(104);
					std::cout << "Sent Mail 104 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(204);
					std::cout << "Sent Mail 204 ..\n";
				}
			}

		}
		
		//=== COMMAND 2 IS FIVE ====//
		else if (Command2 == "5")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(115);
					std::cout << "Sent Mail 115 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{

				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(215);
					std::cout << "Sent Mail 215 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(105);
					std::cout << "Sent Mail 105 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(205);
					std::cout << "Sent Mail 205 ..\n";
				}
			}


		}

		//=== COMMAND 2 IS SIX ====//
		else if (Command2 == "6")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(116);
					std::cout << "Sent Mail 116 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(216);
					std::cout << "Sent Mail 216 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(106);
					std::cout << "Sent Mail 106 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(206);
					std::cout << "Sent Mail 206 ..\n";
				}
			}

		}

		//=== COMMAND 2 IS SEVEN ====//
		else if (Command2 == "7")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(117);
					std::cout << "Sent Mail 117 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(217);
					std::cout << "Sent Mail 217 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(107);
					std::cout << "Sent Mail 107 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(207);
					std::cout << "Sent Mail 207 ..\n";
				}
			}

		}

		//=== COMMAND 2 IS EIGHT ====//
		else if (Command2 == "8")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(118);
					std::cout << "Sent Mail 118 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(218);
					std::cout << "Sent Mail 218 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(108);
					std::cout << "Sent Mail 108 ..\n";
				}
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(208);
					std::cout << "Sent Mail 208 ..\n";
				}
			}

		}

		//=== COMMAND 2 IS NINE ====//
		else if (Command2 == "9")
		{
			//Check if Command1 was 1, 2, u or d
			if (Command1 == "1")			//Send Elevator 1 to Floor 0
			{
				if (elevator1_status.GeneralStatus != FALSE)
				{
					Elevator1.Post(119);
					std::cout << "Sent Mail 119 ..\n";
				}
			}
			else if (Command1 == "2")		//Send Elevator 2 to Floor 0
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(219);
					std::cout << "Sent Mail 219 ..\n";
				}
			}

			else if (Command1 == "u")		//To start, let Elevator 1 deal with UP requests	
			{
				//This means someone is on Floor '9' requesting to go UP this doesn't make sense, ignore this request
				std::cout << "You are on the top floor already, elevator can't go up anymore...\n";
			}

			else if (Command1 == "d")		//To start, let Elevator 2 deal with DOWN requests	
			{
				if (elevator2_status.GeneralStatus != FALSE)
				{
					Elevator2.Post(209);
					std::cout << "Sent Mail 209 ..\n";
				}
			}

		}

		//=== COMMAND 2 IS E ====//
		else if (Command2 == "e")
		{
			if (Command1 == "e")
			{
				Elevator1.Post(505);
				Elevator2.Post(505);
				FINISHED = TRUE;
				//break;
			}
		}

		else
			std::cout << "That is an invalid command\n";

	}




















	t1.WaitForThread();
	t2.WaitForThread();
	t3.WaitForThread();

	UserIO.WaitForProcess();
	Elevator1.WaitForProcess();					// wait for the child process to end
	Elevator2.WaitForProcess();					// wait for the child process to end

	std::cout << "ELEVATOR SIMULATION HAS BEEN EXITED...\n";

	return 0;
}


//==================================================================
// Other spare code
//==================================================================

/*if (Command.find("u") != string::npos)
			{

				std::cout << "Passenger is outside the Elevator \n";

				if (Command.find("0") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 0 \n";
				else if (Command.find("1") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 1 \n";
				else if (Command.find("2") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 2 \n";
				else if (Command.find("3") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 3 \n";
				else if (Command.find("4") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 4 \n";
				else if (Command.find("5") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 5 \n";
				else if (Command.find("6") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 6 \n";
				else if (Command.find("7") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 7 \n";
				else if (Command.find("8") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 8 \n";
				else if (Command.find("9") != string::npos)
					std::cout << "Waiting for elevator, Going to floor 9 \n";
				else
					std::cout << "That floor does not exist\n";
			}
			else
				std::cout << "That is an invalid command!!!\n";*/
				//if (waitForDown != string::npos)