#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "plclib.h"

void plc_log(const char * msg, ...)
{
    va_list arg;
    char msgstr[MAXSTR];
    memset(msgstr,0,MAXSTR);
    va_start(arg, msg);
    vsprintf(msgstr,msg,arg);
    va_end(arg);
    printf("%s\n",msgstr);
}

int project_task(plc_t p)
{
    return 0;
}

int project_init()
{
    return 0;
}

/**stubbed hardware**/

/**
 * @brief fetch all input bytes if necessary
 * @param number of inputs
 * @return error code
 */
int io_fetch(long timeout, unsigned int bytes)
{
    return 0;
}

/**
 * @brief flush all output bytes if necessary
 * @param number of outputs
 * @return error code
 */
int io_flush(unsigned int bytes)
{
    return 0;
}

/**
 * @brief read digital input
 * @param index
 * @param value
 */
void dio_read(const int index ,unsigned char* value)
{
}

/**
 * @brief write bit to digital output
 * @param value
 * @param n index
 * @param bit
 */
void dio_write(const unsigned char *value, const int n, const int bit)
{
}

/**
 * @brief read analog sample
 * @param the index
 * @param the raw value 
 */
void data_read(unsigned int index, uint64_t* value)
{
}

/**
 * @brief write analog sample
 * @param the index
 * @param the raw value 
 */
void data_write(unsigned int index, uint64_t value)
{
}

