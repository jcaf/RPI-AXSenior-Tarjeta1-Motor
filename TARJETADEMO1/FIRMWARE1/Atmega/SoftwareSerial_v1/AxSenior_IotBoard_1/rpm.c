#include "system.h"
#include "main.h"
#include "rpm.h"

volatile uint16_t rpm_ISR;
uint16_t RPM = 0;

void RPM_process(void)
{
    static int8_t t;
    if (main_flag.f20ms )
    {
        if (++t>= 50)//50* 20ms = 1000 //1 segundo
        {
            BitTo0(EIMSK,0);
            RPM = rpm_ISR * 60;//1M
            rpm_ISR = 0x0000;
            BitTo1(EIMSK,0);
            //
            t = 0;
        }
    }
}
