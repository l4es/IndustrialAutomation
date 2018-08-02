#ifndef _TERMIOSH
#define _TERMIOSH

#pragma push_safeptr

#include "sys/stdtypes.h"
#include "sys/termdefs.h"
#include "sys/termios.h"

int tcgetattr(int, struct termios *);
int tcsetattr(int, int, const struct termios *);

speed_t cfgetispeed(const struct termios *);
int cfsetispeed(struct termios *, speed_t);

speed_t cfgetospeed(const struct termios *);
int cfsetospeed(struct termios *, speed_t);

int tcdrain(int);
int tcflow(int, int);
int tcsendbreak(int, int);
int tcflush(int, int);


#pragma pop_ptr

#endif
