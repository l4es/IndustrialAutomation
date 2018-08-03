#ifndef _UTIL_H_
#define _UTIL_H_
/**logging, metrics, debugging stuff */

#define LOG "plcemu.log"
void plc_log(const char * msg, ...);
void close_log();

#endif //_UTIL_H
