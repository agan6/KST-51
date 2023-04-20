#include "config.h"

unsigned char led = 0xfe;

void LEDworking()
{
	ADDR0 = 0;
	ADDR1 = 1;
	ADDR2 = 1;
	ADDR3 = 1;
	ENLED = 0;
	led = ~led | 0xfe;
	P0 = led;
}

void LEDnoworking()
{
	ADDR0 = 0;
	ADDR1 = 1;
	ADDR2 = 1;
	ADDR3 = 1;
	ENLED = 0;
	P0 = 0xFF;
}