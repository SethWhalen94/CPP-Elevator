#include "..\rt.h"
#include "..\MonitorClass.h"
#include "Elevator.h"

//===================================
//Global Rendevous object declaration
//===================================

CRendezvous r2Start("Process_Start", 3);		//These Rendevous with wait for all Processes to start or finish
CRendezvous r2Finish("Process_Finish", 3);		//2 elevators and IO.

	//							GS	   DS	DIR  FLOOR
Status elevator2_status = { TRUE, TRUE, TRUE, 0 };
Monitor m2("E2Status");			//Create monitor class for Elevator 1

CMailbox Elevator2_Mail;	// Create a message queue for this child thread
int message;
BOOL Close_Doors = TRUE;
BOOL Open_Doors = FALSE;
BOOL finished = FALSE;
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
	//Wait for Elevator1 and IO to start
	r2Start.Wait();
	while (finished == FALSE)
	{
		m2.Update_Status(elevator2_status);		//Update elevator status
		Print_Elevator(elevator2_status);		//Print Elevator
		if (Elevator2_Mail.TestForMessage() == TRUE)
		{
			message = Elevator2_Mail.GetMessage();

			switch (message)
			{
			case 200:
				Change_Floor(elevator2_status.Floor, 0);
				break;

			case 201:
				Change_Floor(elevator2_status.Floor, 1);
				break;

			case 202:
				Change_Floor(elevator2_status.Floor, 2);
				break;

			case 203:
				Change_Floor(elevator2_status.Floor, 3);
				break;

			case 204:
				MOVE_CURSOR(0, 0);
				Change_Floor(elevator2_status.Floor, 4);
				break;

			case 205:
				Change_Floor(elevator2_status.Floor, 5);
				break;

			case 206:
				Change_Floor(elevator2_status.Floor, 6);
				break;

			case 207:
				Change_Floor(elevator2_status.Floor, 7);
				break;

			case 208:
				Change_Floor(elevator2_status.Floor, 8);
				break;

			case 209:
				Change_Floor(elevator2_status.Floor, 9);
				break;

			case 210:
				Change_Floor(elevator2_status.Floor, 0);
				break;

			case 211:
				Change_Floor(elevator2_status.Floor, 1);
				break;

			case 212:
				Change_Floor(elevator2_status.Floor, 2);
				break;

			case 213:
				Change_Floor(elevator2_status.Floor, 3);
				break;

			case 214:
				Change_Floor(elevator2_status.Floor, 4);
				break;

			case 215:
				Change_Floor(elevator2_status.Floor, 5);
				break;

			case 216:
				Change_Floor(elevator2_status.Floor, 6);
				break;

			case 217:
				Change_Floor(elevator2_status.Floor, 7);
				break;

			case 218:
				Change_Floor(elevator2_status.Floor, 8);
				break;

			case 219:
				Change_Floor(elevator2_status.Floor, 9);
				break;

			case 500:
				elevator2_status.GeneralStatus = TRUE;		//Put Elevator IN SERVICE
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
				break;

			case 501:
				elevator2_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
				break;

			case 505:
				Change_Floor(elevator2_status.Floor, 0);
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
				finished = TRUE;				//EXIT SIMULATION
				break;
			default:
				break;
			}

		}

	}



	//Wait for other processes to finish
	MOVE_CURSOR(0, 0);
	cout << "Elevator 2 is now Finished the simulation...\n";
	r2Finish.Wait();
	MOVE_CURSOR(0, 1);
	cout << "Elevator 2 is now TERMINATED...\n";
	return 0;		// exit child program by returning status value 0
				// Note we could also call exit(0) to achieve the same thing
}

//Function to print elevator
void Change_Floor(int currentFloor, int DestFloor)
{
	if (currentFloor < DestFloor)
	{
		elevator2_status.DoorStatus = FALSE;		//Close the elevator Door
		elevator2_status.Direction = TRUE;			//Elevator is Going UP
		m2.Update_Status(elevator2_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator2_status);
		for (int i = currentFloor; i < DestFloor; i++)
		{
			SLEEP(1000);
			elevator2_status.Floor = elevator2_status.Floor + 1;	//Increment floor of elevator
			currentFloor = elevator2_status.Floor;					//Update Current Floor
			m2.Update_Status(elevator2_status);						//Update Status for Dispatch and IO
			Print_Elevator(elevator2_status);
			if (Elevator2_Mail.TestForMessage(501, 501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
			{
				elevator2_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
				while (Elevator2_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
				//Update Elevator to IN SERVICE
				elevator2_status.GeneralStatus = TRUE;
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
			}

			Floors_on_the_way(currentFloor);		//Check if there are OUTSIDE requests on the current Floor

		}

		elevator2_status.DoorStatus = TRUE;		//OPEN the elevator Door
		m2.Update_Status(elevator2_status);		//Update status for Dispatch and IO
		Print_Elevator(elevator2_status);		//Print the Elevator
		SLEEP(1000);							//Keep doors open for a sec
	}
	else if (currentFloor > DestFloor)
	{
		elevator2_status.DoorStatus = FALSE;		//Close the elevator Door
		elevator2_status.Direction = FALSE;			//Elevator is Going DOWN
		m2.Update_Status(elevator2_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator2_status);
		for (int i = currentFloor; i > DestFloor; i--)
		{
			SLEEP(1000);
			elevator2_status.Floor = elevator2_status.Floor - 1;	//Increment floor of elevator
			currentFloor = elevator2_status.Floor;					//Update Current Floor
			m2.Update_Status(elevator2_status);						//Update Status for Dispatch and IO
			Print_Elevator(elevator2_status);

			//Check if OUt OF SERVICE mail has been sent
			if (Elevator2_Mail.TestForMessage(501, 501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
			{
				elevator2_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
				while (Elevator2_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
				//Update Elevator to IN SERVICE
				elevator2_status.GeneralStatus = TRUE;
				m2.Update_Status(elevator2_status);
				Print_Elevator(elevator2_status);
			}

			Floors_on_the_way(currentFloor);		//Check if there are OUTSIDE requests on the current Floor

		}
		elevator2_status.DoorStatus = TRUE;		//OPEN the elevator Door
		m2.Update_Status(elevator2_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator2_status);
		SLEEP(1000);							//Keep doors open for a sec
	}
	else //CurrentFloor == DestFloor
	{
		//Already on this floor
		elevator2_status.DoorStatus = TRUE;		//Close the elevator Door
		m2.Update_Status(elevator2_status);			//Update status for Dispatch and IO
		Print_Elevator(elevator2_status);

		//Check if OUt OF SERVICE mail has been sent
		if (Elevator2_Mail.TestForMessage(501, 501) == TRUE)		//Check if OUt OF SERVICE mail has been sent
		{
			elevator2_status.GeneralStatus = FALSE;		//Put Elevator OUT OF SERVICE
			m2.Update_Status(elevator2_status);
			Print_Elevator(elevator2_status);
			while (Elevator2_Mail.TestForMessage(500, 500) == FALSE);	//stay here WHILE no IN SERVICE mail is present
			//Update Elevator to IN SERVICE
			elevator2_status.GeneralStatus = TRUE;
			m2.Update_Status(elevator2_status);
			Print_Elevator(elevator2_status);
		}

	}

}

void Floors_on_the_way(int current_floor)//, int destination_floor)
{
	current_floor = 200 + current_floor;		//Add 100 so we can compare to u0-u9 Mail values To curr and destination floors
	//destination_floor = 100 + destination_floor;
	/*MOVE_CURSOR(0, 0);
	cout << "elevator Direction is " << elevator2_status.Direction << '\n';*/
	if (Elevator2_Mail.TestForMessage(current_floor, current_floor) && elevator2_status.Direction == TRUE)		//Check if there is an OUTSIDE Elevator request on the Current Floor AND that ELEvator is going UP
	{
		Elevator2_Mail.GetMessage();		//Read message to remove it from the queue

		elevator2_status.DoorStatus = TRUE;		//Open the Door so Passenger can get in
		m2.Update_Status(elevator2_status);		//Update status
		Print_Elevator(elevator2_status);
		SLEEP(1000);							//Keep door open for a sec
		elevator2_status.DoorStatus = FALSE;	//Close the Door
		m2.Update_Status(elevator2_status);
		Print_Elevator(elevator2_status);

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
				if (elevator2_status.DoorStatus == TRUE)		//DOOR is OPEN
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
				if (elevator2_status.DoorStatus == TRUE)		//DOOR is OPEN
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

	if (status.DoorStatus == TRUE && Open_Doors == FALSE)
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
	else if (status.DoorStatus == FALSE && Close_Doors == TRUE)
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