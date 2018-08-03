#ifndef _PLCEMU_H_
#define _PLCEMU_H_
/**
 *@file plcemu.h
 *@brief main header
*/

#define PRINTABLE_VERSION 1.6
//analog I/O

#define TERMLEN 80
#define TERMHGT 24

#define KEY_TAB		9

int plc_load_file(const char * path);
int plc_save_file(const char * path);
#endif //_PLCEMU_H_
