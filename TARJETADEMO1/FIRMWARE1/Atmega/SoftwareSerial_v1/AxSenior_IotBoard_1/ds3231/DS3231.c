#include "DS3231.h"
#include "../system.h"
#include "../utils/utils.h"

void DS3231_init(void)
{
    #ifdef _DS3231_USING_RSTCTRL_
    //DS3231_RST_ConfigModeMonitoringVDDifFall();
    DS3231_RST_ConfigModeExternalReset();
    #endif

    #ifdef _DS3231_USING_VDDCTRL_
    ConfigOutputPin(DDRxDS3231_VDDCTRL,DS3231_pinVDDCTRL);
    DS3231_VDD_ON_10msESTABILIZACION(); //+10ms delay Estabilizar correctamente DS3231
    //DS3231_VDD_ON();
    #endif

    DS3231_SET_CONTROLSTATUS_REG((0<<DS3231_OSF_BIT)		|
                                    (0<<DS3231_EN32KHZ_BIT)	|
                                    (0<<DS3231_BSY_BIT)		|
                                    (0<<DS3231_A2F_BIT)		|
                                    (0<<DS3231_A1F_BIT));

    DS3231_SET_CONTROL_REG((0<<DS3231_EOSC_BIT)	|
                              (0<<DS3231_BBSQW_BIT) |
                              (0<<DS3231_CONV_BIT)	|
                              (0<<DS3231_RS2_BIT)	|
                              (0<<DS3231_RS1_BIT)	|
                              (0<<DS3231_INTCN_BIT) |
                              (0<<DS3231_A2IE_BIT)	|
                              (0<<DS3231_A1IE_BIT));
}

/*********************************************************************************
Actualizar al final: I2C_unimaster_tx_data(DS3231_SECONDS_REG);
pues segun datasheet, luego de escribir SECONDS, empieza un nuevo "COUNTDOWN"

The  countdown  chain  is  reset  whenever  the  seconds
register is written. Write transfers occur on the acknowl-
edge  from  the  DS3231.  Once  the  countdown  chain  is
reset,  to  avoid  rollover  issues  the  remaining  time  and
date registers must be written within 1 second. The 1Hz
square-wave output, if enabled, transitions high 500ms
after the seconds data transfer, provided the oscillator
is already running.
*********************************************************************************/


void DS3231_set_time(uint8_t hour, uint8_t min, uint8_t sec, int8_t mode12_24, int8_t am_pm)
{
    uint8_t buff[3];
    uint8_t h;

    buff[0]=dec2bcd(sec);	//sec
    buff[1]=dec2bcd(min);	//min
    //buff[2]=(mode12_24<<DS3231_1224_BIT) | (ampm<<DS3231_AMPM_BIT) | dec2bcd(hour);

    if (mode12_24 == DS3231_24_HOUR_MODE)
        h = (dec2bcd(hour) & 0x3F);
    else
        h = (am_pm<<DS3231_AMPM_BIT) + (dec2bcd(hour) & 0x1F);
    //
    buff[2]= (mode12_24<<DS3231_1224_BIT) + h;

    DS3231_WRITE_ARRAY_TO_REGS(DS3231_SECONDS_REG, buff, 3);
}

void DS3231_set_date(uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    uint8_t buff[4];

    buff[0]=day;
    buff[1]=dec2bcd(date);
    buff[2]=dec2bcd(month);
    buff[3]=dec2bcd(year);
    //
    DS3231_WRITE_ARRAY_TO_REGS(DS3231_DAY_REG, buff, 4);
}

void DS3231_get_time(struct _DS3231_time_bin *time_bin)//rtcc binary
{
	//Unpacking BCD
    uint8_t S10,S;
    uint8_t M10,M;
    uint8_t H10,H;

    uint8_t buff[3];

    DS3231_READ_REGS_TO_ARRAY(DS3231_SECONDS_REG, buff, 3);

    //seconds 00-59
    S10= buff[0]>>4;
    S	= buff[0] & 0x0F;
    time_bin->sec = S10*10 + S;

    //minutes 00-59
    M10= buff[1]>>4;
    M	= buff[1] & 0x0F;
    time_bin->min = M10*10 + M;

	//1-12 + ~AMP/PM or 00-23
	time_bin->h12_24mode = (buff[2] & (1<<DS3231_1224_BIT)) >>DS3231_1224_BIT;

	time_bin->ampm = 0;//init to know value
	if (time_bin->h12_24mode == DS3231_12_HOUR_MODE)
	{
		time_bin->ampm = (buff[2] & (1<<DS3231_AMPM_BIT)) >>DS3231_AMPM_BIT;
		H10= (buff[2]>>4) & 0x01;
	}
	else
	{
		H10= (buff[2]>>4) & 0x03;//H20-H10
	}
    H = buff[2] & 0x0F;
    time_bin->hour = H10*10 + H;
}


void DS3231_get_date(struct _DS3231_date_bin *date_bin)//rtcc binary
{
    uint8_t D10,D;
    uint8_t MM10,MM;
    uint8_t Y10,Y;

    uint8_t buff[4];

    DS3231_READ_REGS_TO_ARRAY(DS3231_DAY_REG, buff, 4);

    date_bin->day = buff[0];//Day 1..7

    //Date Dd
    D10 = (buff[1]>>4) & 0x03;
    D = buff[1] & 0x0F;
    date_bin->date = D10*10 + D;

    //Month Mm //01..12
    MM10 = (buff[2]>>4) & 0x01;
    MM = buff[2] & 0x0F;
    date_bin->month = MM10*10 + MM;

    //Year 00..99
    Y10 = (buff[3]>>4) & 0x0F;//Bug fixed
    Y = buff[3] & 0x0F;
    date_bin->year = Y10*10 + Y;
}


