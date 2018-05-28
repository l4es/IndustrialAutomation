/*
  +-----------------------------------------------------------------------+
  | (C) ADDI-DATA GmbH         Dieselstrasse 3       D-77833 Ottersweier  |
  +-----------------------------------------------------------------------+
  | Tel : +49 (0) 7223/9493-0     | email    : info@addi-data.com         |
  | Fax : +49 (0) 7223/9493-92    | Internet : http://www.addi-data.com   |
  +-------------------------------+---------------------------------------+
  | Project     : -               |     Compiler   : GCC                  |
  | Module name : addidata_type.h |     Version    : 2.96                 |
  +-------------------------------+---------------------------------------+
  | Author      : J. Krauth       |     Date       : 25.06.2004           |
  +-------------------------------+---------------------------------------+
  | Description :                                                         |
  |               ADDI-DATA Type definition.                              |
  +-----------------------------------------------------------------------+
  |                             UPDATES                                   |
  +----------+-----------+------------------------------------+-----------+
  |   Date   |   Author  |  Description of updates            | Version   |
  +----------+-----------+------------------------------------+-----------+
  | 25.06.04 | J. Krauth |  Creation                          | 01.00.00  |
  +----------*-----------*------------------------------------*-----------+
  */

#ifndef ADDIDATATYPES_H
#define ADDIDATATYPES_H

typedef unsigned char  uint8;
//Word
typedef unsigned short int uint16;
//Long
typedef unsigned long   uint32;

// Byte
typedef	char  int8;
// Word
typedef short int int16;
//long
typedef long   int32;

typedef unsigned char BOOL;
typedef unsigned char STATUS;

#define VOID		void
#define CHAR		char

#define BYTE		unsigned char
#define PBYTE		unsigned char *

#define SHORT		short
#define USHORT		unsigned short
#define PUSHORT		unsigned short *

#define INT		int
#define PINT		int *

#define WORD		unsigned int	
#define DWORD		unsigned long	

#define UINT		unsigned int
#define PUINT		unsigned int *

#define LONG		long
#define PLONG		long *

#define ULONG		unsigned long
#define PULONG		unsigned long *

#define LOBYTE(W)	(BYTE)((W) & 0xFF)
#define HIBYTE(W)	(BYTE) (((W) >> 8) & 0xFF)
#define MAKEWORD(H,L)	(USHORT) ((L) | ((H) << 8))
#define LOWORD(W)	(USHORT) ((W) & 0xFFFF)
#define HIWORD(W)	(USHORT) (((W) >> 16) & 0xFFFF)
#define MAKEDWORD(H,L)	(UINT) ((L) | ((H) << 16))


#endif
