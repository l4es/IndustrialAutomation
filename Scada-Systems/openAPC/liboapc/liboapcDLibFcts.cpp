/********************************************************************************************

This program and source file is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.

*********************************************************************************************/

#ifdef ENV_LINUX
 #include <dlfcn.h>
#endif

#include "liboapc.h"



#ifdef ENV_WINDOWSCE
static void toWChar(WCHAR *out, const char *in, int len)
{
   const char *c;

   c=in;
   while ((*c) && (len>0))
   {
      *out=*in;
      out++;
      in++;
      len--;
   }
}
#endif



/**
 */
OAPC_EXT_API void *oapc_dlib_load(const char *name)
{
#ifdef ENV_WINDOWSCE
   WCHAR wname[250+4];

   toWChar(wname,name,250);
   return LoadLibrary(wname);
#else
 #ifdef ENV_WINDOWS
   return LoadLibrary(name);
 #else
  #if defined (ENV_LINUX) || defined (ENV_QNX)
   return dlopen(name,RTLD_LAZY);
  #else
   #error Not implmemented!
    return NULL;
   #endif
 #endif
#endif
}



OAPC_EXT_API void *oapc_dlib_get_symbol(void *handle,const char *name)
{
#ifdef ENV_WINDOWSCE
   WCHAR wname[100+4];

   toWChar(wname,name,100);
   return (void*)GetProcAddress((HMODULE)handle,wname);
#else
 #ifdef ENV_WINDOWS
   return (void*)GetProcAddress((HMODULE)handle,name);
 #else
  #if defined (ENV_LINUX) || defined (ENV_QNX)
   return dlsym(handle,name);
  #else
   #error Not implemented!
   return NULL;
  #endif
 #endif
#endif
}



OAPC_EXT_API void oapc_dlib_release(void *handle)
{
#ifdef ENV_WINDOWS
   FreeLibrary((HMODULE)handle);
#else
 #if defined (ENV_LINUX) || defined (ENV_QNX)
   dlclose(handle);
 #else
  #error Not implemented!
 #endif
#endif
}


