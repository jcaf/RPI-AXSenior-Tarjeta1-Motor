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
#include <stdlib.h>

//#include <pigpio.h>
//-lpigpio -lrt

#include <wiringPi.h>
//-lwiringPi

////////////////////////////////////////////////////////////
#define RX_rxBuffFER_MAXSIZE 64
#define NUM_VALUE2SEND_MAX 4//
float intValue2send[NUM_VALUE2SEND_MAX];

#define CHARVALUE2SEND_STR_MAXSIZE 20
char charValue2send[NUM_VALUE2SEND_MAX][10];
////////////////////////////////////////////////////////////

void parse_rx_str(char *cad);

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
	char rxBuff[RX_rxBuffFER_MAXSIZE];
	
	//sleep(2);
	tcflush(fd, TCIOFLUSH);
	//
	unsigned long nline=0;
	
	while (1)	
	{
		fcntl(fd, F_SETFL, 0);//reset flags
		n = read(fd,(void*)rxBuff, 30);

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
			parse_rx_str(rxBuff);
			printf("%.0fV %.3fA %.1fC %.0fRPM\n", intValue2send[0],intValue2send[1],intValue2send[2],intValue2send[3]);
			printf("%s %s %s %s\n\n", &charValue2send[0][0], &charValue2send[1][0], &charValue2send[2][0], &charValue2send[3][0]);
			
			//option 1, este chip usart usa el \n como fin de linea
			//rxBuff[n-1] = '\0';
			//printf("%lu = %i bytes read : %s\n", ++nline, n, rxBuff);
			//
			//option 2,
			rxBuff[n] = '\0';
			printf("%lu = %i bytes read : %s", ++nline, n, rxBuff);
			
		}
		
		delay(1000);
	}
	close (fd);
	return 0;
}


void parse_rx_str(char *cad)
{
    char intchar[20];
    int posc=0;
    int idx = 0;

    for (int i=0; i<RX_rxBuffFER_MAXSIZE; i++)
    {
        if (cad[i] == '#')
        {
            //reset all
            posc = 0x00;
            idx = 0;
        }
        else
        {
            if ( (cad[i] == ' ') || (cad[i] == '\n'))//break cad
            {
                intchar[idx] = '\0';
                idx = 0x00;

                strcpy(&charValue2send[posc][0], intchar);
                intValue2send[posc++] = strtof( intchar, NULL);

                //
                if ( cad[i] == '\n')//exit
                    {break;}
            }
            else
            {
                intchar[idx] = cad[i];
                idx++;
            }
        }
    }
}
