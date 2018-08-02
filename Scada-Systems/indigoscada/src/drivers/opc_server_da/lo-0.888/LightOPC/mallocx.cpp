/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 mallocX() wrappers
 **************************************************************************/

#include <windows.h>
#include <malloc.h>
#include "util.h"

#if 0 == LO_USE_OBJXREF
LONG lo_X_objcount = -1;
#else
LONG lo_X_objcount;
#endif

#ifndef _HEAPOK
#define _HEAPOK (-2)
#endif

#ifdef _HEAPOK
#define LO_HEAPCHECK(hc) if (_HEAPOK != (hc = _heapchk()))                    \
                           UL_ERROR((LOGID, "loDelete()::heapchk() %d", hc));
#else
#define LO_HEAPCHECK(hc) (hc = 0);
#endif

#if 0 == LO_USE_MALLOCX

long mallocX_count = -1;

#define TRAP_ALLOC(p)
#define TRAP_FREE(p)

int mallocX_trap(void)
{
 int hc;
 LO_HEAPCHECK(hc);
 return hc;
}

#else /* LO_USE_MALLOCX */

long mallocX_count;

#define  LO_USE_MALLOCX_TRAP (0)

#if 0 == LO_USE_MALLOCX_TRAP

#define TRAP_ALLOC(p)  InterlockedIncrement(&mallocX_count)
#define TRAP_FREE(p)   InterlockedDecrement(&mallocX_count)

int mallocX_trap(void)
{
 int hc;
 LO_HEAPCHECK(hc);
 return hc;
}

#else

static void *trm[8000];
static unsigned tra;

void TRAP_ALLOC(void *buf)
{
   if (tra < SIZEOF_ARRAY(trm))
     {
      UL_TRACE((LOGID, "TRA: %u", tra));
      trm[tra++] = buf;
     }
   else UL_ERROR((LOGID, "mallocX TRAP OVERFLOW (%p)", buf));
   mallocX_count++;
}

void TRAP_FREE(void *buf)
{
 unsigned gg;
 for(gg = 0; gg < tra; gg++)
   if (trm[gg] == buf)
     {
      trm[gg] = 0; goto Break;
     }
 UL_ERROR((LOGID, "WRONG free(%p)", buf));
Break:
 mallocX_count--;
}

int mallocX_trap(void)
{
 unsigned gg;
 int hc;

// LO_HEAPCHECK(hc);
 if (_HEAPOK != (hc = _heapchk()))
   UL_ERROR((LOGID, "loDelete()::heapchk() %d", hc));
 else UL_WARNING((LOGID, "loDelete()::heapchk() Ok (%d)", hc));

 for(gg = 0; gg < tra; gg++)
   if (trm[gg])
     {
      UL_ERROR((LOGID, "malloc TRAP %u (%p)", gg, trm[gg]));
      UL_ERROR((LOGID, "malloc TRAP <%.5ls>", trm[gg]));
     }
 return hc;
}

#endif

void *mallocX(unsigned size)
{
 unsigned *buf;
// mallocX_trap();

 size += 4 * sizeof(unsigned) - 1;
 size /= sizeof(unsigned);

 buf = new unsigned[size];

 if (!buf) return 0;

 *buf++ = (size - 1) * sizeof(unsigned);

 TRAP_ALLOC(buf);

 return (void*)buf;
}

void freeX(void *buf)
{
 if (buf)
   {
    TRAP_FREE(buf);
    delete (((unsigned*)buf) - 1);
   }
// mallocX_trap();
}

void *reallocX(void *buf, unsigned size)
{
 unsigned osize;
 void *nbuf;

 if (!buf) return mallocX(size);
 osize = ((unsigned*)buf)[-1];
 if (osize >= size)
   {
    if (size != 0) return buf;
    freeX(buf);
    return 0;
   }
 if (nbuf = mallocX(size))
   {
    memcpy(nbuf, buf, osize);
    freeX(buf);
   }
 return nbuf;
}

void *callocX(unsigned num, unsigned size)
{
 void *buf;
 buf = mallocX(size *= num);
 if (buf) memset(buf, 0, size);
 return buf;
}

#endif /* LO_USE_MALLOCX */
/* end of xmalloc.cpp */
