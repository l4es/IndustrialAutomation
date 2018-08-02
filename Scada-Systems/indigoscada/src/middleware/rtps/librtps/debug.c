/*
 *  $Id: debug.c,v 0.0.0.1              2003/08/21
 *
 *  DEBUG:  section 1                   Debug
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr.smolik@wo.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "orte_all.h"

/* global debug variables */
int db_level;
int debugLevels[MAX_DEBUG_SECTIONS];
int mem_check_counter=0;
NtpTime zNtpTime,iNtpTime;
SequenceNumber noneSN;

/*********************************************************************/
/* forvard declarations */
static void
db_print_output(const char *format);

/*********************************************************************/
/* globals */

#ifndef CONFIG_ORTE_RT
FILE *debug_log=NULL;       /* NULL */
#endif

/*********************************************************************/
/* functions */

#ifdef CONFIG_ORTE_RT
static const char *
debug_log_time(void) {
  struct timespec        time;
  static char            buf[64];

  clock_gettime(CLOCK_REALTIME, &time);
  sprintf(buf, "%li.%03li", time.tv_sec,time.tv_nsec/1000000);
  return buf;
}
#else
static const char *
debug_log_time(void) {
  struct timeval        time;
  static char         buf[64];

  gettimeofday(&time,NULL);
  sprintf(buf, "%li.%03li", time.tv_sec,time.tv_usec/1000);
  return buf;
}
#endif /* CONFIG_ORTE_RT */

void
db_print(const char *format,...) {
  char f[256];
  va_list ap;

  va_start(ap, format);
  sprintf(f, "%s | ",debug_log_time());
  vsprintf(f+strlen(f),format,ap);
  va_end(ap);
  db_print_output(f);    
}

void
db_print_output(const char *format) {
#ifndef CONFIG_ORTE_RT
  if (debug_log == NULL) return;
  fprintf(debug_log, format);
  fflush(debug_log);
#else
  rtl_printf(format);
#endif
}

void
debug_arg(const char *arg) {
  int32_t s=0,l=0,i;

  if (!strncmp(arg, "ALL", 3)) {
    s = -1;
    arg += 4;
  } else {
    s = atoi(arg);
    while (*arg && *arg++ != '.');
  }
  l = atoi(arg);
  if (l < 0) l = 0;
  if (l > 10) l = 10;
  if (s >= 0) {
    debugLevels[s] = l;
    return;
  }
  for (i = 0; i < MAX_DEBUG_SECTIONS; i++)
    debugLevels[i] = l;
} 

void 
debug_options(const char *options) {
  char *p = NULL;
  char *s = NULL;

  if (options) {
    p=(char*)MALLOC(strlen(options)+1);
    if (p) {
      memcpy(p, options, strlen(options) + 1);
    }    
    for (s = strtok(p, ":"); s; s = strtok(NULL, ":"))
      debug_arg(s);
    FREE(p);
  }     
}

void
debug_open_log(const char *logfile) {
#ifndef CONFIG_ORTE_RT
  if (logfile == NULL) {
    debug_log = stderr;
    return;
  }
  if (debug_log && debug_log != stderr)
    fclose(debug_log);
  debug_log = fopen(logfile, "a+");
  if (!debug_log) {
    fprintf(stderr, "WARNING: Cannot write log file: %s\n", logfile);
    perror(logfile);
    fprintf(stderr, "         messages will be sent to 'stderr'.\n");
    fflush(stderr);
    debug_log = stderr;
  }
#endif
}

void
db_init(const char *logfile, const char *options) {
  int i;

  for (i = 0; i < MAX_DEBUG_SECTIONS; i++)
    debugLevels[i] = -1;
  debug_options(options);
  debug_open_log(logfile);
}

#ifdef ENABLE_MEM_CHECK
void *mem_check_malloc(size_t size) { 
  void *ptr;
  
  if ((ptr=malloc(size))) {
    mem_check_counter++;
    debug(1,9) ("mem check: inc %d\n",mem_check_counter);
  }
  return ptr;
}

void mem_check_free(void *ptr) {
  if(!ptr) {
//    LOG_FATAL(KERN_CRIT "ul_mem_check_free : triing to free NULL ptr\n");
  }else{
    mem_check_counter--;
    debug(1,9) ("mem check: dec %d\n",mem_check_counter);
    free(ptr);
  }
}
#endif /* ENABLE_MEM_CHECK */
