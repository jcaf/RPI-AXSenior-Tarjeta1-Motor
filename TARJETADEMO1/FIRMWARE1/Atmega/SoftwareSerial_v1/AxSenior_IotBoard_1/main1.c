/*
 * SW_Serial.c
  */

//#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "system.h"
#include <util/delay.h>

#include "SWseriale.h" // SWseriale library
// SWseriale example
// Standard settings:
// RX -> Pin 3
// TX -> Pin 4
#include "usart/usart.h"
#include "voltage.h"
#include "main.h"
#include "adc/adc.h"

extern float volt;

volatile struct _isr_flag
{
    unsigned f20ms:1;
    unsigned __a:7;
} isr_flag = {0};

struct _main_flag main_flag = {0};

int main(void)
{
    char outbuff[20];
    unsigned int ii=0;

    uint16_t cdelay=0;

    //UART by hardware
    //USART_Init(MYUBRR );//@9600
    USART_Init(47);//@9600
    int i;
    while (1)
    {
        //ii++;
        //itoa(ii,outbuff,10);
                usart_print_string("AMOR\n");
        //strcat(outbuff,"\n");
        //usart_print_string(outbuff);
        //_delay_ms(2000);
        //for (i=0;i<10; i++)
        __delay_ms(2000);
        //if (ii%10 != 0)
        //    _delay_ms(2000);
        //else
        //    _delay_ms(1000*5);
    }



    voltage_init();
    ADC_init(ADC_MODE_SINGLE_END);//only for INA129
    //////////////////////////////////////////////
    #define CTC_SET_OCRnA(CTC_FREQ, CTC_PRESCALER) ( (uint8_t)( (F_CPU/ (2.0*CTC_PRESCALER*CTC_FREQ)) -1) )//q la division sea entre decimals
    TCNT1 = 0x0000;//if no zero, -> t= 1/(16e6/1024) -> t*(65535-77) = 4.18s
    TCCR1A=0;
    TCCR1B = (1 << WGM12) | (1 << CS12) |(0 << CS11)|(0 << CS10);//CTC PRES=256
    OCR1A = 1249;//CTC_SET_OCRnA( 25, 64);// 25Hz-> cada 20ms , prees=256 --> 1249
    TIMSK1 |= (1<<OCIE1A);//TIMSK1 = (1<<OCIE1A);
    //////////////////////////////////////////////

    //
	sei();
//SWseriale_begin(); // Initialize INT1, Timer2, Pin 3 (Input, Pull-up) and Pin 4 (Output)
	while (1)
    {
        if (isr_flag.f20ms)
        {
            isr_flag.f20ms = 0;
            //
            main_flag.f20ms = 1;
        }
        /////////////////////////

        process_voltage();//udpate internamente volt

        if (main_flag.f20ms)
        {
            if (++cdelay >= 100)//20*100 = 2000
            {
                cdelay = 0;

                ii = (unsigned int)volt;
                //ii++;
                itoa(ii, outbuff, 10);
                strcat(outbuff,"\n");
                //SWseriale_write( &outbuff, strlen(outbuff) );
                usart_print_string(outbuff);

            }
        }
        //
        main_flag.f20ms = 0;    //clear flag!
    }

/*
    while (1)
    {
	while (SWseriale_available()){ // Checks if any character has been received
	    uint8_t temp = SWseriale_read(); // Reads one character from SWseriale received data buffer
	    SWseriale_write(&temp, 1); // Send one character using SWseriale
    }

    _delay_ms(10); // Wait 10 ms, optional
    }
*/
}


//(16e6 /(2*512*25))-1 //CTC ->624
ISR(TIMER1_COMPA_vect)//cada 20ms
{
    //    static int8_t f10ms_counter;
    //
    //    if (++f10ms_counter==10)
    //    {
    //        f10ms_counter=0x00;
    isr_flag.f20ms = 1;
    //    }
    //PID_control_output(PID_out_as_dutycycle);
}
