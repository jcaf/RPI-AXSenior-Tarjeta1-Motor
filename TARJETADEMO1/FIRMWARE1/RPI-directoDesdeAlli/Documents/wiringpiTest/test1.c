#include <iostream>
#include <wiringPi.h>

int main(void)
{
	wiringPiSetup();
	pinMode(15, OUTPUT);
	for (;;)
	{
		digitalWrite(15, HIGH);
		delay(500);
		digitalWrite(15, LOW);
		delay(500);
		
	}
	return 0;
	
}
