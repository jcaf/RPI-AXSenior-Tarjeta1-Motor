/*

PL2303 (Prolific): 75 to 1.2M baud, not including 250k, 500k and 1M, 256 RX / 256 TX FIFO

@16Mhz 57600 baud its ok

ads1115 @860hz -> 14.33 samples
1) 698//0
2) 549//
3) 1145//
4) 1011//-->>>esta es la mitad
5) 2503//
6) 6284//mitad mas algo
7) 2113
-------------------
8) -2154
9) -1420
10) -1154
11) -967
12) -1553
13) -5838
14) -3303

 */
//#define F_CPU 7372800UL //HARDWARE
//#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <string.h>
#include <stdlib.h>
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

#include "types.h"
#include "i2c/I2C.h"
#include "i2c/I2CCommonFx.h"
#include "ads1115/ads1115.h"
#include "corriente.h"
extern float volt;

volatile struct _isr_flag
{
    unsigned f20ms:1;
    unsigned __a:7;
} isr_flag = {0};

struct _main_flag main_flag = {0};


/*
union
    {
        uint8_t reg[2];
        uint16_t ;
    };
*/
//#define SOFTSERIAL
#ifdef SOFTSERIAL
    #define serialPrint(str,strlen) do{SWseriale_write( str, strlen);}while(0)
#else
    #define serialPrint(str,strlen) do{usart_print_string(str);}while(0)
#endif // SOFTSERIAL
int main(void)
{
    uint16_t cdelay=0;
    char outbuff[20];
    unsigned int ii=0;
    char str[40];

    //UART by hardware
    USART_Init ( MYUBRR );//@9600
    /*
    while (1)
    {
        ii++;
        itoa(ii,outbuff,10);
                //usart_print_string("1234\n");
        strcat(outbuff,"\n");
        //usart_print_string(outbuff);
        serialPrint(outbuff, strlen(outbuff));

        _delay_ms(1000);

        //if (ii%10 != 0)
        //    _delay_ms(2000);
        //else
        //    _delay_ms(1000*5);
    }
    */

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

	SWseriale_begin(); // Initialize INT1, Timer2, Pin 3 (Input, Pull-up) and Pin 4 (Output)
    sei();

    ConfigOutputPin(DDRD,7);//PD7 AS OUTPUT FOR LED!


    strcpy(str,"INICIALIZANDO I2C\n");
    //SWseriale_write( (uint8_t *)str, strlen(str));
    serialPrint( (uint8_t *)str, strlen(str) );

    _delay_ms(1000);
    ///////////////
    I2C_unimaster_init(100E3);//100KHz
    uint8_t reg[2];
    uint16_t b16;
    //++--Write config
    reg[0] = (1<<OS_BIT) | (MUX_AIN0_AIN1<<MUX_BIT) | (PGA_4p096V<<PGA_BIT) | (CONTINUOUS_CONV<<MODE_BIT);
    reg[1] = (DR_860SPS<<DR_BIT);

    I2Ccfx_WriteArray(ADS115_ADR_GND,ADS1115_CONFIG_REG, &reg[0], 2);
    //--++
    b16 = (reg[0]<<8) + reg[1];
    utoa(b16, outbuff, 10);
    strcpy(str,"Write ConfigReg:\n");
    strcat(str,outbuff);
    strcat(str,"\n");
    //SWseriale_write( (uint8_t *)str, strlen(str) );
    serialPrint( (uint8_t *)str, strlen(str) );

    _delay_ms(10);
    //++--
    //Default is 0x8583 = 34179
    //0x85 = 133
    //0x83 = 131
    I2Ccfx_ReadRegistersAtAddress(ADS115_ADR_GND,ADS1115_CONFIG_REG, &reg[0], 2);
    b16 = (reg[0]<<8) + reg[1];
    utoa(b16, outbuff, 10);

    strcpy(str,"Read ConfigReg: \n");
    strcat(str,outbuff);
    strcat(str,"\n");

    //SWseriale_write( (uint8_t *)str, strlen(str) );
    serialPrint( (uint8_t *)str, strlen(str) );

    _delay_ms(10);
    //--++

    //default state of ConfigRegister = 0x8583 = 34179
    //_delay_ms(3000);
    int16_t ib16;
//    while (1)
//    {
//        /*
//        //read byte x byte
//        I2Ccfx_ReadRegistersAtAddress(ADS115_ADR_GND,ADS1115_CONFIG_REG, &reg[0], 2);
//        utoa(reg[0], outbuff, 10);
//        strcat(outbuff,"\n");
//        SWseriale_write( &outbuff, strlen(outbuff) );//133
//        //
//        utoa(reg[1], outbuff, 10);
//        strcat(outbuff,"\n");
//        SWseriale_write( &outbuff, strlen(outbuff) );//131
//        */
//
//        I2Ccfx_ReadRegistersAtAddress(ADS115_ADR_GND,ADS1115_CONVRS_REG, &reg[0], 2);
//        ib16 = (reg[0]<<8) + reg[1];
//        itoa(ib16, outbuff, 10);
//        strcat(outbuff,"\n");
//        //SWseriale_write( (uint8_t *)outbuff, strlen(outbuff) );
//        serialPrint( (uint8_t *)outbuff, strlen(outbuff) );
//
//        _delay_ms(1);
//        //_delay_ms(2000); // Wait 10 ms, optional
//    }

//++++++++++++++++++++++++++++++++++++++++++++++
    while (1)
    {
        if (isr_flag.f20ms)
        {
            isr_flag.f20ms = 0;
            main_flag.f20ms = 1;
        }
        /////////////////////////
        //process_voltage();//udpate internamente volt
        Irms_get();
        if (main_flag.f20ms)
        {
            if (++cdelay >= 50)//20*100 = 2000
            {
                cdelay = 0;
                //
                dtostrf(irms,0,3,outbuff);
                strcat(outbuff,"\n");
                serialPrint( (uint8_t *)outbuff, strlen(outbuff) );

                //
                PinToggle(PORTD,7);
            }
        }
        //
        main_flag.f20ms = 0;    //clear flag!
    }

//++++++++++++++++++++++++++++++++++++++++++++++
	while (1)
    {
        ii++;
        itoa(ii, outbuff, 10);

        strcat(outbuff,"\n");

        //strcpy(outbuff, "1.2\n");

        SWseriale_write( &outbuff, strlen(outbuff) );
        _delay_ms(2000); // Wait 10 ms, optional
    }


    /* Replace with your application code */
    while (1)
    {
	while (SWseriale_available()){ // Checks if any character has been received
	    uint8_t temp = SWseriale_read(); // Reads one character from SWseriale received data buffer
	    SWseriale_write(&temp, 1); // Send one character using SWseriale
    }

    _delay_ms(10); // Wait 10 ms, optional
    }
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

