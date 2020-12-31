#ifndef __Passenger__
#define __Passenger__
#include "..\rt.h"
//
//CSemaphore fuel("Fuel_car", 0, 1);
//CSemaphore light("Pit_entry_light", 15, 15);

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
			
		}
		return 0;
	}

public:
	Passenger(int number, int curr, int dest) : MyNumber(number), CurrentFloor(curr), Destination(dest)
	{

	}

};


#endif
