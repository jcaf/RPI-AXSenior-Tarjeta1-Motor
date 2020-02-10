/*
 * Disable getty
 * -------------
 
 Puede ser serial-getty@ o getty@
 * 
 * sudo systemctl stop getty@ttyS0 (Stop)
 * 
 * sudo systemctl disable serial-getty#ttyS0.service (Disable)
 * 
 * 
 * Usar tcdrain() instead of tcflush()
 * */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

//#include <pigpio.h>
//-lpigpio -lrt

#include <wiringPi.h>
//-lwiringPi

int main(void)
{
	/*
	if (gpioInitialise()  <0)
	{
		return 1;
	}
	*/ 
	wiringPiSetup();
	///////////////////////////////////// serial port //////////////////
	int fd;
	fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	//fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		perror("open_port: unable to open");
		return -1;
	}
	else 
	{
		perror("OK");
		}
	//set port, default baud = 9600
	struct termios options;
	tcgetattr(fd, &options);
	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);
	tcsetattr(fd,TCSANOW, &options);
	//
	fcntl(fd, F_SETFL, 0);//reset flags
	////////////////////////////////////////////////////////////////////
	
	//Write to the port
	int n = write(fd, "12345678901234567890\n",21);
	if (n < 0)
	{
		perror("Write failed -");
		return -1;
	}
	char buf[6];
	
	//sleep(2);
	tcflush(fd, TCIOFLUSH);
	//
	unsigned long nline=0;
	
	while (1)	
	{
		//printf("LEYENDO\n");
		
		fcntl(fd, F_SETFL, 0);//reset flags
		//perror("WHILE");
		////////////////////// read serial port buffer /////////////////
		//n = read(fd,(void*)buf, 12);//ok leyendo hasta el domingo 9feb2020
		n = read(fd,(void*)buf, 30);
		//n = read(fd, buf, 12);
		
		
		if (n < 0)
		{
			perror("read failed");
			return -1;
		}
		else if (n == 0) 
		{
			printf("No data on port\n");
		}
		else
		{
			buf[n-1] = '\0';
			printf("%lu = %i bytes read : %s\n", ++nline, n, buf);
		}
		///////////////////////////////////////////////////////////////
		
		//gpioDelay(1000*100);//1s
		delay(1000);
	}
	
	//close the port
	close (fd);
	//gpioTerminate();
	return 0;
}
