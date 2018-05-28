#ifndef _MSR_MEM_H_
#define _MSR_MEM_H_

#ifdef __KERNEL__                                                                                
#define getmem(size) vmalloc((size))
#define freemem(item) vfree(item)
#define freeandnil(item) do { vfree(item); item = NULL; } while(0)
#else                       
#define getmem(size) malloc(size)
#define freemem(item) free(item)
#define freeandnil(item) do { free(item); item = NULL; } while(0)
#endif
#endif
