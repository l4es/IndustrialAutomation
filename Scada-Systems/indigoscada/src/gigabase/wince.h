#ifndef __WINCE_H__
#define __WINCE_H__

#include <winsock.h>

#include "wince_time.h"

#define assert(x) 
#define stricmp   _stricmp
#define getenv(x) 0

int abort();

#define ASM_CPUID_NOT_SUPPORTED 1

#endif
