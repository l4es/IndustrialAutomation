/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 loGrowingList implementation
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "growlist.h"
#include "util.h"

#ifndef glGrowList_init

void glGrowList_init(glGrowingList *gl)
{
 gl->gl_list = 0;
 gl->gl_count = 0;
  /* private part */
 gl->gl_total_count = 0;
 gl->gl_first_free = 0;
}
#endif

void glGrowList_clear(glGrowingList *gl)
{
 gl->gl_first_free = 0;
 gl->gl_count = gl->gl_total_count = 0;
 if (gl->gl_list)
   {
    void **nlist = gl->gl_list;
    gl->gl_list = 0;
    freeX(nlist);
   }
}

static unsigned gl_find_slot(glGrowingList *gl)
{
 unsigned from;
 for(from = gl->gl_first_free; from < gl->gl_count; from++)
   if (!gl->gl_list[from]) return (gl->gl_first_free = from) + 1;
 gl->gl_first_free = gl->gl_count;
 return 0;
}

unsigned gl_find(glGrowingList *gl, void *element)
{
 unsigned from;
 for(from = 0; from < gl->gl_count; from++)
   if (element == gl->gl_list[from]) return from + 1;
 return 0;
}

unsigned gl_grow(glGrowingList *gl, unsigned newcount) /* grows the list
   at least to newcount and return new count if ok or 0 if failed */
{
 if (newcount <= gl->gl_count) return gl->gl_total_count;
 if (newcount > gl->gl_total_count)
   {
    unsigned adcount;

    adcount = gl->gl_total_count;
    adcount = adcount + (adcount >> 4);

    if (newcount > adcount) adcount = newcount;
    if (!preallocX((void**)&gl->gl_list, sizeof(gl->gl_list[0]) * adcount))
      return 0; /* E_OUTOFMEMORY; */

    memset(gl->gl_list + gl->gl_total_count, 0,
            (newcount - gl->gl_total_count) * sizeof(gl->gl_list[0]));
    gl->gl_total_count = adcount;
   }
 return gl->gl_count = newcount;
}

unsigned gl_insert(glGrowingList *gl, void *element, unsigned position)
{
 if (!element) return 0;
 if (!position && !(position = gl_find_slot(gl)))
   {
    if (!gl_grow(gl, position = gl->gl_count + 1)) return 0;
   }
 else if (position > gl->gl_count)
   {
    if (!gl_grow(gl, position)) return 0;
   }
 else if (gl->gl_list[position - 1])
   {
    unsigned slot;

    if (!(slot = gl_find_slot(gl)) &&
        !gl_grow(gl, slot = gl->gl_count + 1)) return 0;
    if (slot < position)
      memmove(gl->gl_list + slot - 1, gl->gl_list + slot,
              sizeof(gl->gl_list[0]) * (position - slot));
    else
      memmove(gl->gl_list + position, gl->gl_list + position - 1,
              sizeof(gl->gl_list[0]) * (slot - position));
   }
 gl->gl_list[position - 1] = element;
 return position;
}

void *gl_remove(glGrowingList *gl, unsigned position)
{
 void *element = 0;
 if (position > 0 &&
     position <= gl->gl_count &&
     (element = gl->gl_list[position - 1]))
   {
    gl->gl_list[position - 1] = 0;
    if (position == gl->gl_count)
      {
       while(position && !gl->gl_list[position - 1]) position--;
       gl->gl_count = position;
      }
    if (gl->gl_first_free > position - 1)
      gl->gl_first_free = position - 1;
   }
 return element;
}

/* end of growlist.cpp */

