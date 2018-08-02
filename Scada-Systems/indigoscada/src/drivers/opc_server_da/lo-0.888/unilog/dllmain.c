/******************  Universal EventLogging utility  ****************
 **                                                                **
 *                    Copyright (c) 1996,1999 by Timofei Bondarenko *
 ********************************************************************/
#include <windows.h>
/*#include "unilog.h"*/

#ifdef __cplusplus
extern "C" {
#endif
extern void unilog_CloseDefault(void);
extern void unilog_advinit(void);
extern void unilog_advdestroy(void);
#ifdef __cplusplus
           }
extern "C"
#endif
extern HMODULE unilog_dll_module;

BOOL APIENTRY DllMain(HANDLE module,
                      DWORD  reason,
                      LPVOID reserved)
{
    switch(reason)
        {
    case DLL_PROCESS_ATTACH:
         unilog_dll_module = module;
         DisableThreadLibraryCalls(module);
         unilog_advinit();
         break;
    case DLL_PROCESS_DETACH:
         unilog_advdestroy();
         unilog_CloseDefault();
         break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
         break;
        }
    return TRUE;
}

/* end of dllmain.c */
