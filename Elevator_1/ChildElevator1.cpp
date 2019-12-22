#include "..\rt.h"
#include "..\MonitorClass.h"
#include "Elevator.h"

//TEXT_COLOURS
//0 - Black
//1 - Dark Blue
//2 - Dark Green
//3 - Dark Cyan
//4 - Dark Red
//5 - Dark Magenta
//6 - Dark Yellow
//7 - Grey
//8 - Black(again)
//9 - Blue
//10 - Green
//11 - Cyan
//12 - Red
//13 - Magenta
//14 - Yellow
//15 - White


//struct Status
//{
//	BOOL GeneralStatus;		//TRUE == Working, FALSE == BROKEN
//	BOOL DoorStatus;		//TRUE == OPEN, FALSE == CLOSED
//	BOOL Direction;			//TRUE == UP, FALSE == DOWN
//	int Floor;
//
//};

//===================================
//Global Rendevous object declaration
//===================================
Monitor m1("E1Status");			//Create monitor class for Elevator 1
	//						GS	   DS	DIR  FLOOR
Status elevator1_status = { TRUE, TRUE, TRUE,	0};

CMailbox Elevator1_Mail;	// Create a message queue for this child thread
CRendezvous r2Start("Process_Start", 3);		//These Rendevous with wait for all Processes to start or finish
CRendezvous r2Finish("Process_Finish", 3);		//2 elevators and IO.
int message;
BOOL Close_Doors = TRUE;	//Used for Close Doors animation
BOOL Open_Doors = FALSE;	//Used for Open Doors animation
BOOL finished = FALSE;		//Bool for simulation being finished
//===================
//Function Prototypes
//===================
void Print_Elevator(Status);
void Change_Floor(int, int);
void Floors_on_the_way(int);
void Open_Doors_animation();
void Close_Doors_animation();

int main()
{
	//Wait for Elevator2 and IO to start
	r2Start.Wait();

	while (finished == FALSE)
	{
		m1.Update_Status(elevator1_status);		//Update elevator status
		Print_Elevator(elevator1_status);		//Print Elevator
		if (Elevator1_Mail.TestForMessage() == TRUE)
		{
			message = Elevator1_Mail.GetMessage();

			switch (message)
			{
			case 100:
				Change_Floor(elevator1_status.Floor, 0);
				break;

			case 101:
				Change_Floor(elevator1_status.Floor, 1);
				break;

			case 102:
				Change_Floor(elevator1_status.Floor, 2);
				break;

			case 103:
				Change_Floor(elevator1_status.Floor, 3);
				break;

			case 104:
				Change_Floor(elevator1_status.Floor, 4);
				break;

			case 105:
				Change_Floor(elevator1_status.Floor, 5);
				break;

			case 106:
				Change_Floor(elevator1_status.Floor, 6);
				break;

			case 107:
				Change_Floor(elevator1_status.Floor, 7);
				break;

			case 108:
				Change_Floor(elevator1_status.Floor, 8);
				break;

			case 109:
				Change_Floor(elevator1_status.Floor, 9);
				break;

			case 110:
				Change_Floor(elevator1_status.Floor, 0);
				break;

			case 111:
				Change_Floor(elevator1_status.Floor, 1);
				break;

			case 112:
				Change_Floor(elevator1_status.Floor, 2);
				break;

			case 113:
				Change_Floor(elevator1_status.Floor, 3);
				break;

			case 114:
				Change_Floor(elevator1_status.Floor, 4);
				break;

			case 115:
				Change_Floor(elevator1_status.Floor, 5);
				break;

			case 116:
				Change_Floor(elevator1_status.Floor, 6);
				break;

			case 117:
				Change_Floor(elevator1_status.Floor, 7);
				break;

			case 118:
				Change_Floor(elevator1_status.Floor, 8);
				break;

			case 119:
				Change_Floor(elevator1_status.Floor, 9);
				break;

			case 500:
				elevator1_status.GeneralStatus = TRUE;		//Put Elevator IN SERVICE
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
				break;

			case 501:
				elevator1_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
				break;

			case 505:
				Change_Floor(elevator1_status.Floor, 0);
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
				finished = TRUE;				//EXIT SIMULATION
				break;
			default:
				break;
			}

		}

	}

	

	//Wait for other processes to finish
	MOVE_CURSOR(0, 0);
	cout << "Elevator 1 is now Finished the simulation...\n";
	r2Finish.Wait();

	return 0;		// exit child program by returning status value 0
				// Note we could also call exit(0) to achieve the same thing
}





//=============================
// Other Functions for Elevator
//==============================

void Change_Floor(int currentFloor, int DestFloor)
{
	if (currentFloor < DestFloor)
	{
		elevator1_status.DoorStatus = FALSE;		//Close the elevator Door
		elevator1_status.Direction = TRUE;			//Elevator is Going UP
		m1.Update_Status(elevator1_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator1_status);
		for (int i = currentFloor; i < DestFloor; i++)
		{
			SLEEP(1000);
			elevator1_status.Floor = elevator1_status.Floor + 1;	//Increment floor of elevator
			currentFloor = elevator1_status.Floor;					//Update Current Floor
			m1.Update_Status(elevator1_status);						//Update Status for Dispatch and IO
			Print_Elevator(elevator1_status);
			if (Elevator1_Mail.TestForMessage(501,501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
			{
				elevator1_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
				while (Elevator1_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
				//Update Elevator to IN SERVICE
				elevator1_status.GeneralStatus = TRUE;
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
			}

			Floors_on_the_way(currentFloor);		//Check if there are OUTSIDE requests on the current Floor

		}

		elevator1_status.DoorStatus = TRUE;		//OPEN the elevator Door
		m1.Update_Status(elevator1_status);		//Update status for Dispatch and IO
		Print_Elevator(elevator1_status);		//Print the Elevator
		SLEEP(1000);							//Keep doors open for a sec
	}
	else if (currentFloor > DestFloor)
	{
		elevator1_status.DoorStatus = FALSE;		//Close the elevator Door
		elevator1_status.Direction = FALSE;			//Elevator is Going DOWN
		m1.Update_Status(elevator1_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator1_status);
		for (int i = currentFloor; i > DestFloor; i--)
		{
			SLEEP(1000);
			elevator1_status.Floor = elevator1_status.Floor - 1;	//Increment floor of elevator
			currentFloor = elevator1_status.Floor;					//Update Current Floor
			m1.Update_Status(elevator1_status);						//Update Status for Dispatch and IO
			Print_Elevator(elevator1_status);

			//Check if OUt OF SERVICE mail has been sent
			if (Elevator1_Mail.TestForMessage(501, 501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
			{
				elevator1_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
				while (Elevator1_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
				//Update Elevator to IN SERVICE
				elevator1_status.GeneralStatus = TRUE;
				m1.Update_Status(elevator1_status);
				Print_Elevator(elevator1_status);
			}

			Floors_on_the_way(currentFloor);		//Check if there are OUTSIDE requests on the current Floor

		}
		elevator1_status.DoorStatus = TRUE;		//OPEN the elevator Door
		m1.Update_Status(elevator1_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator1_status);
		SLEEP(1000);							//Keep doors open for a sec
	}
	else //CurrentFloor == DestFloor
	{
		//Already on this floor
		elevator1_status.DoorStatus = TRUE;		//Close the elevator Door
		m1.Update_Status(elevator1_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator1_status);

		//Check if OUt OF SERVICE mail has been sent
		if (Elevator1_Mail.TestForMessage(501, 501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
		{
			elevator1_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
			m1.Update_Status(elevator1_status);
			Print_Elevator(elevator1_status);
			while (Elevator1_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
			//Update Elevator to IN SERVICE
			elevator1_status.GeneralStatus = TRUE;
			m1.Update_Status(elevator1_status);
			Print_Elevator(elevator1_status);
		}

	}

}

void Floors_on_the_way(int current_floor)//, int destination_floor)
{
	current_floor = 100 + current_floor;		//Add 100 so we can compare to u0-u9 Mail values To curr and destination floors
	//destination_floor = 100 + destination_floor;
	/*MOVE_CURSOR(0, 0);
	cout << "elevator Direction is " << elevator1_status.Direction << '\n';*/
	if (Elevator1_Mail.TestForMessage(current_floor,current_floor) && elevator1_status.Direction == TRUE)		//Check if there is an OUTSIDE Elevator request on the Current Floor AND that ELEvator is going UP
	{
		Elevator1_Mail.GetMessage();		//Read message to remove it from the queue

		elevator1_status.DoorStatus = TRUE;		//Open the Door so Passenger can get in
		m1.Update_Status(elevator1_status);		//Update status
		Print_Elevator(elevator1_status);
		SLEEP(1000);							//Keep door open for a sec
		elevator1_status.DoorStatus = FALSE;	//Close the Door
		m1.Update_Status(elevator1_status);
		Print_Elevator(elevator1_status);

	}
}

void Print_Elevator(Status status)
{
	
	
	
	MOVE_CURSOR(25, 0);
	printf("____________________________________________________");

	MOVE_CURSOR(25, 1);
	if (status.GeneralStatus == TRUE) //Elevator is in service
	{
		TEXT_COLOUR(15);		//Make color white
		printf("|                   ");
		TEXT_COLOUR(10);		//Make color green
		printf("IN SERVICE");
		TEXT_COLOUR(15);		//Make color white
		printf("                     |");
	}
	else //Elevator is out of service
	{
		printf("|                 ");
		TEXT_COLOUR(12);		//Make color red
		printf("OUT OF SERVICE");
		TEXT_COLOUR(15);		//Make color white
		printf("                   |");
	}

	TEXT_COLOUR(15);		//Make color White again
	for (int i = 1; i < 3; i++)
	{
		MOVE_CURSOR(25, 1 + i);
		if (i == 2)
		{	
			if (status.Direction == TRUE)
			{
				if (elevator1_status.DoorStatus == TRUE)		//DOOR is OPEN
				{
					printf("|                       %d            UP      ", status.Floor);
					TEXT_COLOUR(12);//4
					printf("*");
					TEXT_COLOUR(15);
					printf("     |");
				}
				else
					printf("|                       %d            UP            |", status.Floor);
			}
			else
			{
				if (elevator1_status.DoorStatus == TRUE)		//DOOR is OPEN
				{
					printf("|                       %d            DOWN      ", status.Floor);
					TEXT_COLOUR(12);//4
					printf("*");
					TEXT_COLOUR(15);
					printf("   |");
				}
				else
					printf("|                       %d            DOWN          |", status.Floor);
			}
		}
		else
			printf("|                                                  |");
	}
	MOVE_CURSOR(25, 4);
	printf("____________________________________________________");

	if (status.DoorStatus == TRUE && Open_Doors ==FALSE)
	{	//For loop to print Doors being open
		for (int i = 1; i < 20; i++)
		{
			MOVE_CURSOR(25, 4 + i);
			printf("|   |                                         |    |");
		}

		//Close_Doors = TRUE;	//Run Door animation on next Closing of Doors
	}
	else if (status.DoorStatus == TRUE && Open_Doors == TRUE)
	{
		Open_Doors_animation();
		Open_Doors = FALSE;
		Close_Doors = TRUE;

	}
	else if(status.DoorStatus == FALSE && Close_Doors == TRUE)
	{	
		Close_Doors_animation();
		Close_Doors = FALSE;		//Stop Door animation now
		Open_Doors = TRUE;
		
	}
	else
	{
		for (int i = 1; i < 20; i++)
		{
			MOVE_CURSOR(25, 4 + i);
			printf("|                       |                          |");
		}
	}
	MOVE_CURSOR(25, 24);
	printf("____________________________________________________");
	fflush(stdout);

}

