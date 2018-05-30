#include "xgeneral.h"
#include "ethercattype.h"

void ec_dump_string(uint8_t *c,int len)
{
	int i ;

	for  (i = 0 ; i < len ; i++){
		printf("%02X ", c[i]);
	}
	puts("");
}

#ifndef __MAKE_DEBUG__
void ec_printf(const char *str, ...){}
#endif
