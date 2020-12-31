#ifndef __Passenger__
#define __Passenger__
#include "..\rt.h"

CMutex pipeM("pipeline_mutex");		//Used to make sure passengers don't write to pipeline at the same time

class Passenger : public ActiveClass
{
private:

	int MyNumber;		//Number to keep track of each Passenger
	int CurrentFloor;
	int Destination;

	int main(void)
	{
		while (1)
		{
			pipeM.Wait();
			//Write PickUpOne() to Dispatcher Pipeline
			//Write PickUpTwo() to Dispatcher Pipeline
			pipeM.Signal();

			while(elevator1_status.Floor != CurrentFloor && elevator1_status.DoorStatus() != 1)	//Wait for elevator to get to current floor, and open its doors
			{ }

			pipeM.Wait();
			//Write DropOffOne() to Dispatcher Pipeline
			//Write DropOffTwo() to Dispatcher Pipeline
			pipeM.Signal();

			while (elevator1_status.Floor != Destination && elevator1_status.DoorStatus() != 1)	//Wait for elevator to get to Destination floor, and open its doors
			{
			}

			//Terminate Passenger Thread
			break;
			
		}
		
		cout << "Passenger #" << MyNumber << "Has reached their floor and terminated...\n";
		return 0;
	}

public:
	Passenger(int number, int curr, int dest) : MyNumber(number), CurrentFloor(curr), Destination(dest)
	{

	}

	string PickUpOne()		//Returns the first command for elevator pickup, Will be " u, d"
	{
		if (MyNumber % 2 == 0)
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

		if (MyNumber % 2 == 0)
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


#endif
