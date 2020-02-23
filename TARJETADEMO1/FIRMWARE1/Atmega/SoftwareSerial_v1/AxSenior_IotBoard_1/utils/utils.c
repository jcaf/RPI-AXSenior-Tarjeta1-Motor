#include "utils.h"
uint8_t dec2bcd(uint8_t dec)
{
    return ( (dec/10)<<4 ) + (dec%10);
}

char bin_to_asciihex(char c)//nibbleBin_to_asciiHex
{
    if (c < 10)
        return c+'0';
    else
        return (c-10) + 'A';
}
//0 1 2 3 4 5 6 7 8 9 A B C D E F
uint8_t asciihex2bin(char asciihex)
{
    if ( (asciihex >= '0') && (asciihex <= '9'))
        return  asciihex-'0';
    else if ((asciihex >= 'A') && (asciihex <= 'F'))
        return  (asciihex-'A') + 10;
    else return 0;
}
uint16_t asciihexBuff2bin(char *ascii_hex, uint8_t num_dig)//1F = 31
{
    uint16_t hex_power;
    uint16_t acc = 0;
    uint8_t num_max = num_dig;
    for (int8_t i=0; i<num_max; i++)
    {
        hex_power=1;
        num_dig--;
        for (int8_t h=0; h<num_dig; h++)
            {hex_power *= 16;}

        int8_t r = asciihex2bin(ascii_hex[i]);
        acc += (r*hex_power);
        //printf("r es: %i, hex_power es %i, acc es %i\n", r,hex_power,acc);
    }
    return acc;
}
void dtostrf_fill_zero(char *p)
{
    while (*p !='.')
    {
        if (*p == ' ')
            *p='0';
        p++;
    }
}


