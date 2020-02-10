#include <avr/io.h>
#include "system.h"
#include "main.h"
#include "voltage.h"
#include "adc/adc.h"
///////////////// VOLTAGE /////////
int adc_max=750;        //Reemplazar por valor adc_max entregado por el sketch: volt_ac_cal
int adc_min=268;        //Reemplazar por valor adc_min entregado por el sketch: volt_ac_cal
float volt_multi=225;   //Reemplazar por el "voltaje ac rms" entregado por un multimetro
float volt_multi_p;
float volt_multi_n;

////////////
void voltage_init(void)
{
    volt_multi_p = volt_multi * 1.4142;   //Voltaje pico= Voltaje RMS * 1.4142 (Corriente Monofasica)
    volt_multi_n = volt_multi_p * -1;
}

#define sq(x) ((x)*(x))
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float Sumatoria=0;
float volt = 0;
unsigned long N=0;

//float
void process_voltage(void)
{
    float adc_sample;
    float volt_inst=0;

    static int volt_Tintegrt = 0;

    //
    adc_sample = ADC_read(ADC_CH_0);//analogRead(A0);    //voltaje del sensor

    volt_inst = map(adc_sample,adc_min,adc_max,volt_multi_n,volt_multi_p);

    Sumatoria = Sumatoria+sq(volt_inst);    //Sumatoria de Cuadrados
    N = N+1;

    //_delay_ms(1);// NO SE desea aplicar este delay

    if (main_flag.f20ms )
    {
        if (++volt_Tintegrt >= 25)//25* 20ms = 500
        {
            volt_Tintegrt = 0x00;
            //
            volt=sqrt((Sumatoria)/N); //ecuación del RMS

            //reset variables
            N = 0x00;
            Sumatoria = 0x00;
        }
    }

    //return(volt);
}
