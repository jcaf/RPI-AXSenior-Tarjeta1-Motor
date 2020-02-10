//-lpigpio -lrt
#include <stdio.h>
#include <pigpio.h>
#define LED 23

int main(void)
{
	if (gpioInitialise()<0) return 1;
	
	gpioSetMode(LED, PI_OUTPUT);
	
	
	for (;;)
	{
		gpioWrite(LED,1);
		gpioDelay(1000*500);
		gpioWrite(LED,0);
		gpioDelay(1000*500);
	}
	gpioTerminate();
	return 0;
}

