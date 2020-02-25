/*
FIRWAR E.I.R.L
Firmware/Software Embedded design
Juan Carlos Aguero Flores


main3.c

Todo el proyecto movido a la carpeta del proyecto, para tener todo unificado


TTL UART TO USB: PL2303 (Prolific): 75 to 1.2M baud, not including 250k, 500k and 1M, 256 RX / 256 TX FIFO

VOLTAGE SENSOR:
CURRENT SENSOR:
TEMPERATURE SENSOR: PT100-WZP
RPM SENSOR: NJK-5002C

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

    read fuses:
    http://www.engbedded.com/fusecalc/

    lock bits:
    http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p

    -booloader size al minimo
    -boot reset disable
    -boden= on, con 2.7V
    -crystal con el max. tiempo de power-on-reset
    -clock ouput= disable
    -divide clock by8 internally = disable
    -resetiopin== no programado.. sigue sirviendo como reset
    -debug wire = disable

    1) DESPROTEGER LA PROTECCION DE BORRADO DE LA EEPROM
    EESAVE DISABLE
    EEPROM memory is preserved through chip erase
    [root@JCAFPC Release]# avrdude -c usbasp -B10 -p m328p -U lfuse:w:0xff:m -U hfuse:w:0xdf:m
    [root@JCAFPC Release]# avrdude -c usbasp -B10 -p m328p -U

    avrdude -c usbasp -B10 -p m328p -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xfd:m

    avrdude -c dragon_isp -B 0.3 -P usb -p atmega328p -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0xfd:m

    2) GRABAR EL CODIGO FUENTE CON EL COMANDO ACOSTUMBRADO
    [root@JCAFPC Release]# avrdude -c usbasp -B5 -p m328p -U flash:w:atmega328p.hex
    [root@JCAFPC Release]# avrdude -c usbasp -B5 -p m328p -V -U flash:w:atmega328p.hex (SIN VERIFICAR)
    [jcaf@JCAFPC Release]$ avrdude -c usbasp -B5 -p m328p (ONLY A RESET)

    avrdude -c dragon_isp -B 0.3 -P usb -p atmega328p -V -U -U flash:w:
    NUEVO
    [root@JCAFPC Release]# avrdude -c usbasp -B0.3 -p m328p -V -U flash:w:atmega328p.hex (MAS RAPIDO!)
    Tambien puede ser sin -BX.. cuando ya esta bien configurado los fuses:
    [root@JCAFPC Release]# avrdude -c usbasp -p m328p -U flash:w:atmega328p.hex

    3) GRABAR LA EEPROM
    [root@JCAFPC Release]# avrdude -c usbasp -B5 -p m328p -U eeprom:w:atmega328p.eep

    4) REESTABLER LA PROTECCION DE LA EEPROM
    EESAVE ENABLE
    [jcaf@JCAFPC Release]$ avrdude -c usbasp -B10 -p m328p -U lfuse:w:0xff:m -U hfuse:w:0xd7:m
    EJECUTE PROGRAMA (1)
    EDITE PROGRAMA (1)
    avr-size -C --mcu=atmega328p atmega328p.elf
    AVRDRAGON!!!

    [root@JCAFPC Release]# avrdude -c dragon_isp -B 0.3 -P usb -p atmega328p -V -U flash:w:atmega328p.hex
    AVRDUDE: En post-instalation... Release, add:
    sudo visudo
    Then edit that file to add to the very end:
    username ALL = NOPASSWD: /fullpath/to/command, /fullpath/to/othercommand
    eg
    jcaf ALL = NOPASSWD: /usr/bin/avrdude
    sudo avrdude -c usbasp -B0.25 -p m328p -V -U flash:w:$(TARGET_OUTPUT_FILE) $(TARGET_OUTPUT_DIR)$(TARGET_OUTPUT_BASENAME).hex

    Read flash (hex format)
    avrdude -c usbasp -B10 -p m328p -U flash:r:leeme.hex:i

    proteger flash (modo 3): lectura y escritura
    avrdude -c usbasp -B10 -p m328p -U lock:w:0xFC:m
    (ignorar el error de 0x3C... pues los 2 bits de mayor peso no estan implentados)

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
#include "ds3231/DS3231.h"
#include "ads1115/ads1115.h"
#include "corriente.h"
#include "temperatura.h"
#include "rpm.h"

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

int main(void)
{
    int16_t voltRMS;
    uint16_t cdelay=0;
    char outbuff[20];
    char str[64];

    //+-
    struct _DS3231_time_bin time_bin;
    struct _DS3231_date_bin date_bin;
    char buff[5];
    //char strTimeDate[30];
    //-+

    ConfigInputPin(DDRxINT0, PIN_INT0);
    PinTo1(PORTWxINT0, PIN_INT0);//pullup x NJK-5002C

    //UART by hardware
    USART_Init ( MYUBRR );//@9600


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
    //Enable falling edge INT0
    BitTo1(EICRA, 1);
    BitTo0(EICRA, 0);
    BitTo1(EIMSK,0);//Enabling Interrupt
    //////////////////////////////////////////
    //SWseriale_begin(); // Initialize INT1, Timer2, Pin 3 (Input, Pull-up) and Pin 4 (Output)
    sei();
    /////////////////////////////////
    ConfigOutputPin(DDRC,3);//PC3 AS OUTPUT FOR LED!

    ///////////////
    I2C_unimaster_init(100E3);//100KHz
    uint8_t reg[2];

    DS3231_init();
    //DS3231_set_date(DS3231_MONDAY_BCD,24,02,20);
    //DS3231_set_time(18,42,0, DS3231_24_HOUR_MODE, DS3231_PM);


    //++--Write config
    reg[0] = (1<<OS_BIT) | (MUX_AIN0_AIN1<<MUX_BIT) | (PGA_4p096V<<PGA_BIT) | (CONTINUOUS_CONV<<MODE_BIT);
    reg[1] = (DR_860SPS<<DR_BIT);
    I2Ccfx_WriteArray(ADS115_ADR_GND,ADS1115_CONFIG_REG, &reg[0], 2);
    //default state of ConfigRegister = 0x8583 = 34179
//I2C_unimaster_error_handler(0xA8); while (1);

    while (1)
    {
        if (isr_flag.f20ms)
        {
            isr_flag.f20ms = 0;
            main_flag.f20ms = 1;
        }
        /////////////////////////

        process_voltage();//udpate internamente volt
//PinTo1(PORTC,3);
        Irms_get();
//PinTo0(PORTC,3);//;LED
        temp_process();
        RPM_process();

        //Send to RPI
        if (main_flag.f20ms)
        {
            if (++cdelay >= 100)//20*100 = 2000
            {
                cdelay = 0;


                //
                strcpy(str,"#");
                voltRMS = (unsigned int)volt;
                itoa(voltRMS, outbuff, 10);
                strcat(str, outbuff);
                strcat(str, " ");

                //RMS Current
                dtostrf(irms,0,2,outbuff);
                strcat(str,outbuff);
                strcat(str," ");

                //temperature
                dtostrf(temperature,0,1,outbuff);
                strcat(str,outbuff);
                strcat(str," ");

                //RPM
                utoa(RPM, outbuff, 10);
                strcat(str, outbuff);
                //strcat(str, "\n");
                strcat(str, " ");
                //serialPrint( (uint8_t *)str, strlen(str) );

                //
                //PinToggle(PORTC,3);

                //added RTC

                DS3231_get_time(&time_bin);
                DS3231_get_date(&date_bin);

                //time:"23:12:21", hour="23/25/20"
                itoa(time_bin.hour, buff, 10);
                strcat(str, buff);
                strcat(str, ":");

                itoa(time_bin.min, buff, 10);
                strcat(str, buff);
                strcat(str, ":");

                itoa(time_bin.sec, buff, 10);
                strcat(str, buff);
                strcat(str, " ");
                //
                itoa(date_bin.date, buff, 10);
                strcat(str, buff);
                strcat(str, "/");

                itoa(date_bin.month, buff, 10);
                strcat(str, buff);
                strcat(str, "/");

                itoa(date_bin.year, buff, 10);
                strcat(str, buff);

                strcat(str, "\n");
                serialPrint(str, strlen(str));

                //

            }
        }
        //
        main_flag.f20ms = 0;    //clear flag!
    }
}

//(16e6 /(2*512*25))-1 //CTC ->624
ISR(TIMER1_COMPA_vect)//cada 20ms
{
    isr_flag.f20ms = 1;
}
//falling edge INT0
ISR(INT0_vect)
{
    rpm_ISR++;
}

