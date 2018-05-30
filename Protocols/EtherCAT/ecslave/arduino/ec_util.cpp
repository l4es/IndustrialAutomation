#include "xgeneral.h"
#include <Arduino.h>
extern "C"
{

uint16_t htons(uint16_t t)
{
	uint16_t low  = t & 0x00FF;
	uint16_t high = t & 0xFF00;
	return (low << 8) | (high >> 8); 
}

uint32_t ecat_local_time()
{
	return 0;
}

void ecat_calibrate_localtime(unsigned long *dummy){}

void ecat_wake_timer()
{
}

void ecat_set_rx_time(void *dummy)
{
}

struct ether_header *eth_hdr(uint8_t *buf)
{
	return (struct ether_header *)buf;
}

void *xmalloc(int size)
{
	Serial.println(size);
	return malloc(size);
}

}
