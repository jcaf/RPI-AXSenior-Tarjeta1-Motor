#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//char cad[]="#220V 0.324A 42.9C 1740RPM?"
char cad[]="#220 0.324 42.9 1740\n";

#define RX_BUFFER_MAXSIZE 64
#define NUM_VALUE2SEND_MAX 4//
float intValue2send[NUM_VALUE2SEND_MAX];

#define CHARVALUE2SEND_STR_MAXSIZE 20
char charValue2send[NUM_VALUE2SEND_MAX][10];

void parse_rx_str(void)
{
    char intchar[20];
    int posc=0;
    int idx = 0;

    for (int i=0; i<RX_BUFFER_MAXSIZE; i++)
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
int main()
{
    parse_rx_str();
    printf("%.0fV %.3fA %.1fC %.0fRPM\n", intValue2send[0],intValue2send[1],intValue2send[2],intValue2send[3]);
    printf("%s %s %s %s\n", &charValue2send[0][0], &charValue2send[1][0], &charValue2send[2][0], &charValue2send[3][0]);
    return 0;
}
