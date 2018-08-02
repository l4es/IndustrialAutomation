#ifdef ENV_WINDOWS
 #include <windows.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#ifndef ENV_WINDOWSCE
#include <errno.h>
#endif

#ifndef VA_COPY
# ifdef HAVE_VA_COPY
#  define VA_COPY(dest, src) va_copy(dest, src)
# else
#  ifdef HAVE___VA_COPY
#   define VA_COPY(dest, src) __va_copy(dest, src)
#  else
#   define VA_COPY(dest, src) (dest) = (src)
#  endif
# endif
#endif

int vasprintf(char **str, const char *fmt, va_list ap)
{
   int     ret=-1;
   va_list ap2;
   char   *string;
   size_t  len;

   len=(size_t)(strlen(fmt)*1.2);
   if ((string =(char*)malloc(len)) == NULL) goto fail;
   for (;;)
   {
      VA_COPY(ap2, ap);
      ret = _vsnprintf(string,len, fmt, ap2);
      if ((ret >= 0) && ((size_t)ret <len))
      {
         *str = string;
         va_end(ap2);
         return (ret);
      } 
      else 
      {        
         // bigger than initial, realloc allowing for nul 
         len+=strlen(fmt);
         if ((string=(char*)realloc(string, len)) == NULL) 
         {
            free(string);
            goto fail;
         } 
      }
      va_end(ap2);
   }
fail:
   *str = NULL;
#ifndef ENV_WINDOWSCE
   errno = ENOMEM;
#endif
   va_end(ap2);
   return (-1);
}



int asprintf(char **str, const char *fmt, ...)
{
        va_list ap;
        int ret;
        
        *str = NULL;
        va_start(ap, fmt);
        ret = vasprintf(str, fmt, ap);
        va_end(ap);

        return ret;
}
