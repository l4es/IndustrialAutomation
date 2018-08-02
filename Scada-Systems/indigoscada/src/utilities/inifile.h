/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef INIFILE_H___
#define INIFILE_H___

#include <stdio.h>
#include <string.h>

#define INIFILE_MAX_LINELEN 256 /* max number of chars in a line */

#define COMMENT_CHAR ';'        /* signifies a comment */

typedef struct
{
  char tag[INIFILE_MAX_LINELEN];
  char rest[INIFILE_MAX_LINELEN];
} INIFILE_ENTRY;

#ifndef UTILITIES_DLL_EXP_IMP
	#ifdef WIN32
		#ifdef UTILITIES_EXPORTS
		#define UTILITIES_DLL_EXP_IMP  __declspec(dllexport)
		#else 
		#define UTILITIES_DLL_EXP_IMP __declspec(dllimport)
		#endif
	#else
		// under UNIX we have no such thing as exports
		#define UTILITIES_DLL_EXP_IMP 
	#endif
#endif

class UTILITIES_DLL_EXP_IMP Inifile
{
 public:
  Inifile();
  Inifile(const char *path);
  ~Inifile();

  const int open(const char *path);
  const int close();
  const char *find(const char *tag, const char *section = NULL);
  int section(const char *section,
              INIFILE_ENTRY array[],
              int max);
  const int valid();

 private:

  FILE *fp;
};

#endif /* INIFILE_H___ */
