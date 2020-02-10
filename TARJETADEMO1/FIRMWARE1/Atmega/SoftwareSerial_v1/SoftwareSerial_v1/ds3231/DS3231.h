#ifndef DS3231_H_
#define DS3231_H_

/*************************************************
The slave address byte contains the 7-bit DS3231 address, which is 1101000
*************************************************/
#define DS3231_SLAVE_ADDRESS	(0xD0)

/*************************************************
VDD CONTROL

Descomentar si se usa un pin de microprocesor como VDD
Mas adelante se define el pin de CONTROL DE VDD

NOTA: Generar delay x estabilizar correctamente DS3231
*************************************************/
//#define _DS3231_USING_VDDCTRL_

#ifdef _DS3231_USING_VDDCTRL_

    #define PORTxDS3231_VDDCTRL	(PORTD)
    #define DDRxDS3231_VDDCTRL	(DDRD)
    #define PINxDS3231_VDDCTRL	(PIND)
    //
    #define DS3231_pinVDDCTRL	(4) //PD4
    //
    #define DS3231_VDD_ON()		do{PinTo1(PORTxDS3231_VDDCTRL,DS3231_pinVDDCTRL);}while(0)
    #define DS3231_VDD_ON_10msESTABILIZACION() do{PinTo1(PORTxDS3231_VDDCTRL,DS3231_pinVDDCTRL);_delay_ms(10);}while(0)

    #define DS3231_VDD_OFF()	do{PinTo0(PORTxDS3231_VDDCTRL,DS3231_pinVDDCTRL);}while(0)

#endif
/*************************************************
    ~RST CONTROL

Descomentar si se usa un pin de microprocesor como RST
Mas adelante se define el pin de CONTROL DE RST
The RST pin has an internal 50kÎ© (nominal) pullup resistor to VCC.
*************************************************/
//#define _DS3231_USING_RSTCTRL_

#ifdef _DS3231_USING_RSTCTRL_

    #define PORTxDS3231_RSTCTRL	(PORTG)
    #define DDRxDS3231_RSTCTRL	(DDRG)
    #define PINxDS3231_RSTCTRL	(PING)
    //
    #define DS3231_pinRSTCTRL	(1)	//PG1
    //
    #define DS3231_RST_HIGH()		do{PinTo1(PORTxDS3231_RSTCTRL,DS3231_pinRSTCTRL);}while(0)
    #define DS3231_RST_LOW()		do{PinTo0(PORTxDS3231_RSTCTRL,DS3231_pinRSTCTRL);}while(0)

    //ModeMonitoringVDDFall: por defecto deja el pin en entrada + pull-up desactivado.DS3231 integra su propia resistencia.
    //El programa en modo "ModeExternalReset" DEBERIA dejar el pin RST en alto,,,,para que el cambio sea "transparente", por un ciclo
    //se vera el pin como ENTRADA+Pullup, luego procede a desactivarlo
    #define DS3231_RST_ConfigModeMonitoringVDDifFall() do{ConfigInputPin(DDRxDS3231_RSTCTRL,DS3231_pinRSTCTRL);PinTo0(PORTxDS3231_RSTCTRL,DS3231_pinRSTCTRL);}while(0)

    //ModeExternalReset:Por defecto deja el pin en Alto. Entrada+Pull-up intermediario para pasar a pin de salida en alto.
    #define DS3231_RST_ConfigModeExternalReset() do{PinTo1(PORTxDS3231_RSTCTRL,DS3231_pinRSTCTRL);ConfigOutputPin(DDRxDS3231_RSTCTRL,DS3231_pinRSTCTRL);}while(0)

#endif
/*************************************************
Timekeeping register

Note: Unless otherwise specified, the registersÂ’ state
is not defined when power is first applied.

*************************************************/
#define DS3231_SECONDS_REG			(0x00)
#define DS3231_MINUTES_REG			(0x01)
#define DS3231_HOURS_REG			(0x02)
#define DS3231_DAY_REG				(0x03)
#define DS3231_DATE_REG				(0x04)
#define DS3231_MONTHCENTURY_REG		(0x05)//The century bit(7) is toggled when the years register overflows from 99 to 00.
#define DS3231_YEAR_REG				(0x06)
//
#define DS3231_ALARM1_SECONDS_REG	(0x07)
#define DS3231_ALARM1_MINUTES_REG	(0x08)
#define DS3231_ALARM1_HOURS_REG		(0x09)
#define DS3231_ALARM1_DAYDATE_REG	(0x0A)
//
#define DS3231_ALARM2_MINUTES_REG	(0x0B)
#define DS3231_ALARM2_HOURS_REG		(0x0C)
#define DS3231_ALARM2_DAYDATE_REG	(0x0D)	//Day/Date

//
#define DS3231_CONTROL_REG			(0x0E)
#define DS3231_CONTROLSTATUS_REG	(0x0F)	//Control/Status
#define DS3231_AGINGOFFSET_REG		(0x10)
#define DS3231_MSBTEMP_REG			(0x11)
#define DS3231_LSBTEMP_REG			(0x12)

/*************************************************
    BIT POSITION
*************************************************/
//Hour register
#define DS3231_1224_BIT	(6)	//1->12  0->24
#define DS3231_AMPM_BIT	(5)

//Month register
#define DS3231_CENTURY_BIT	(7)

//Commons
#define DS3231_A1M1_BIT	(7)
#define DS3231_A1M2_BIT	(7)
#define DS3231_A1M3_BIT	(7)
#define DS3231_A1M4_BIT	(7)
//
#define _DS3231_A2M2_	(7)
#define _DS3231_A2M3_	(7)
#define _DS3231_A2M4_	(7)
//
#define DS3231_DYDT_BIT	(6)
//
#define DS3231_SIGN_BIT	(7)

//Control register
#define DS3231_EOSC_BIT		(7)
#define DS3231_BBSQW_BIT	(6)
#define DS3231_CONV_BIT		(5)
#define DS3231_RS2_BIT		(4)
#define DS3231_RS1_BIT		(3)
#define DS3231_INTCN_BIT	(2)
#define DS3231_A2IE_BIT		(1)
#define DS3231_A1IE_BIT		(0)

//Control/Status register
#define DS3231_OSF_BIT		(7)
#define DS3231_EN32KHZ_BIT	(3)
#define DS3231_BSY_BIT		(2)
#define DS3231_A2F_BIT		(1)
#define DS3231_A1F_BIT		(0)

/**************************************************
DS3231 INTERNAL RANGE
**************************************************/
#define DS3231_SUNDAY_BCD       (0x01)
#define DS3231_MONDAY_BCD       (0x02)
#define DS3231_TUESDAY_BCD      (0x03)
#define DS3231_WEDNESDAY_BCD    (0x04)
#define DS3231_THURSDAY_BCD     (0x05)
#define DS3231_FRIDAY_BCD       (0x06)
#define DS3231_SATURDAY_BCD     (0x07)
//
#define DS3231_JANUARY_BCD	    (0x01)
#define DS3231_FEBRUARY_BCD	    (0x02)
#define DS3231_MARCH_BCD		(0x03)
#define DS3231_APRIL_BCD		(0x04)
#define DS3231_MAY_BCD		    (0x05)
#define DS3231_JUNE_BCD		    (0x06)
#define DS3231_JULY_BCD		    (0x07)
#define DS3231_AUGUST_BCD		(0x08)
#define DS3231_SEPTEMBER_BCD    (0x09)
#define DS3231_OCTOBER_BCD	    (0x10)
#define DS3231_NOVEMBER_BCD	    (0x11)
#define DS3231_DECEMBER_BCD	    (0x12)

/*************************************************
Other defs.
*************************************************/
#define DS3231_24_HOUR_MODE 0
#define DS3231_12_HOUR_MODE 1

#define DS3231_AM 0
#define DS3231_PM 1

/*************************************************
MACROS
*************************************************/
#include "I2CCommonFx.h"

#define DS3231_SET_CONTROLSTATUS_REG(DATA) do{I2Ccfx_Write1Byte(DS3231_SLAVE_ADDRESS, DS3231_CONTROLSTATUS_REG,DATA);}while(0)
#define DS3231_SET_CONTROL_REG(DATA) do{I2Ccfx_Write1Byte(DS3231_SLAVE_ADDRESS, DS3231_CONTROL_REG,DATA);}while(0)
#define DS3231_WRITE_ARRAY_TO_REGS(START_ADDRESS,pDATA,NUMBYTES_TOWRITE) do{I2Ccfx_WriteArray(DS3231_SLAVE_ADDRESS,START_ADDRESS, pDATA, NUMBYTES_TOWRITE);}while(0)
#define DS3231_READ_REGS_TO_ARRAY(START_ADDRESS, pDATA, NUMBYTES_TOREAD) do{I2Ccfx_ReadRegistersAtAddress(DS3231_SLAVE_ADDRESS,START_ADDRESS, pDATA, NUMBYTES_TOREAD);}while(0)
struct _DS3231_rtcc_bin
{
	int8_t sec;
	int8_t min;
	int8_t hour;
	int8_t h12_24mode;
	int8_t ampm;
	//
	int8_t day;
	int8_t date;
	int8_t month;
	int16_t year;
};

void DS3231_init(void);
void DS3231_set_datetime(void);
void DS3231_get_datetime(struct _DS3231_rtcc_bin * rtcc_bin);//rtcc binary

#endif /* DS3231_H_ */
