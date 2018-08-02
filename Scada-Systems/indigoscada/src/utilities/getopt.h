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

#ifndef __getopt_h__
#define __getopt_h__

#ifdef WIN32

#ifdef __cplusplus
extern "C" {
#endif
	
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
int getopt(int argc, char* const *argv, const char *optstr);

#ifdef __cplusplus
}
#endif

#endif

#endif
