#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

struct _main_flag
{
    unsigned f20ms:1;
    unsigned alarm:1;
    unsigned __a:6;
};

extern struct _main_flag main_flag;


//#define SOFTSERIAL
#ifdef SOFTSERIAL
#define serialPrint(str,strlen) do{SWseriale_write( str, strlen);}while(0)
#else
#define serialPrint(str,strlen) do{usart_print_string(str);}while(0)
#endif // SOFTSERIAL


#endif // MAIN_H_INCLUDED
