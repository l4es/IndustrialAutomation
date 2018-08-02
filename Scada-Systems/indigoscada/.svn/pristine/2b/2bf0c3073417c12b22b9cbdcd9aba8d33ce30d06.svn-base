/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 IsNAN detection
 **************************************************************************/

#ifndef  NAN_H
#define  NAN_H

#ifndef DBL_DIG
#include <float.h>
#endif

#if ((DBL_MANT_DIG != 53) || (DBL_DIG != 15) || (DBL_MAX_10_EXP != 308))
# error Unknown layout of 'double'
#define lo_IsNANd(dbl) (_isnan(dbl))
#else
#if 1
#define lo_IsNANd(dbl) (0x7ff0 == (0x7ff0 & ((short*)&(dbl))[3]))
#else
#define lo_IsNANd(dbl) (0x7f == (0x7f & ((char*)&(dbl))[7]) && \
                        0xf0 == (0xf0 & ((char*)&(dbl))[6]))
#endif
#endif

#if ((FLT_MANT_DIG != 24) || (FLT_DIG != 6) || (FLT_MAX_10_EXP != 38))
# error Unknown layout of 'float'
#define lo_IsNANf(flt) (_isnan(flt))
#else
#if 1
#define lo_IsNANf(flt) (0x7f80 == (0x7f80 & ((short*)&(flt))[1]))
#else
#define lo_IsNANf(flt) (0x7f == (0x7f & ((char*)&(flt))[3]) && \
                        0x80 == (0x80 & ((char*)&(flt))[2]))
#endif
#endif

#endif /*NAN_H*/

