/*  OpenDAX - An open source data acquisition and control system 
 *  Copyright (c) 2007 Phil Birkelbach
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <opendax.h>

#ifndef __FUNC_H
#define __FUNC_H

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define ABS(a)     (((a) < 0) ? -(a) : (a))


/* Wrappers for system calls */
ssize_t xwrite(int fd, const void *buff, size_t nbyte);

/* Memory management functions.  These are just to override the
 * standard memory management functions in case I decide to do
 * something createive with them later. */

void *xmalloc(size_t);
void *xrealloc(void *, size_t);
void *xcalloc(size_t, size_t);
void xfree(void *);

/* Error handling and logging functions */
/* If this is defined the output goes to the system log otherwise it
   goes to STDOUT and STDERR */
//#define DAX_LOGGER

void xfatal(const char *, ...);
void xerror(const char *, ...);
void set_log_topic(u_int32_t);
void xlog(u_int32_t ,const char *, ...);

/* Portability functions */

char *xstrcpy(const char *);
char *xstrdup(char *);

/* Stuff */

int daemonize(char *);

#endif /* !__FUNC_H */
