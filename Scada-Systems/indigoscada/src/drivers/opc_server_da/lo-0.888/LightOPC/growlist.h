/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
 **************************************************************************/

#if !defined(GROWLIST_H)
#define GROWLIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
    {
     void    **gl_list;
     unsigned  gl_count;
/* private part */
     unsigned gl_total_count;
     unsigned gl_first_free;
    } glGrowingList;

void glGrowList_init(glGrowingList *gl);

#define glGrowList_init(gl) (memset(gl,0,sizeof(glGrowingList)))

void glGrowList_clear(glGrowingList *gl);


    /* positions are from 1 to count; 0 is invalid position */
unsigned gl_insert(glGrowingList *gl, void *element, unsigned position);
void    *gl_remove(glGrowingList *gl, unsigned position);
unsigned gl_find(glGrowingList *gl, void *element);

unsigned gl_grow(glGrowingList *gl, unsigned newcount);
/* grows the list at least to addcount
   and return new count if ok or 0 if failed */

#ifdef __cplusplus
           }
#endif
#endif /*GROWLIST_H*/
