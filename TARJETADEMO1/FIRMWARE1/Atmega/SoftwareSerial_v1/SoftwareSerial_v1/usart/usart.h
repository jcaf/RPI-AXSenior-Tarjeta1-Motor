/*
 * usart.h
 *
 * Created: 12/30/2016 11:22:12 PM
 *  Author: jcaf
 */


#ifndef USART_H_
#define USART_H_

void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void USART_Flush( void );

//#include "../system.h"

//#define F_CPU 16000000UL
#define FOSC F_CPU // Clock Speed

#define BAUD 9600//9600 @16mhz ok
//#define BAUD 38400//38400 @16mhz ok
//#define BAUD 115200 // 115200 @16mhz BAD

//#define BAUD 57600// 57600 @16mhz OK


#define MYUBRR FOSC/16/BAUD-1

void usart_print_string(const char *p);
void usart_print_PSTRstring(const char *p);
void usart_println_string(const char *p);
void usart_println_PSTRstring(const char *p);


#endif /* USART_H_ */
