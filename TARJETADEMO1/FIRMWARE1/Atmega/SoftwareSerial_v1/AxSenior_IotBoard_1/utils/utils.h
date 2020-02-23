#ifndef UTILS_H_
#define UTILS_H_
    #include <stdint.h>
    uint8_t dec2bcd(uint8_t dec);
    char bin_to_asciihex(char c);
	uint8_t asciihex2bin(char asciihex);
	uint16_t asciihexBuff2bin(char *ascii_hex, uint8_t num_dig);
    void dtostrf_fill_zero(char *p);
#endif // UTILS


