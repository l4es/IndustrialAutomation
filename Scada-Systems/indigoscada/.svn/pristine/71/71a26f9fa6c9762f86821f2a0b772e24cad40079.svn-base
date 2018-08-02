/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 dllmain()
 **************************************************************************/
#include <windows.h>
#include "util.h"
/*unilog *lolog;*/

#ifdef __cplusplus
extern "C"
#endif

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
 if (fdwReason == DLL_PROCESS_ATTACH)
   {
    DisableThreadLibraryCalls(hinstDLL);
#if 0
    ss = unilog_Open("|LightOPC","%!T",
                    0, /* Max filesize: -1 unlimited, -2 -don't change */
                    ll_DEBUG); /* level [ll_FATAL...ll_DEBUG] */
#elif 0
    lolog = unilog_Create("LightOPC", "|LightOPC","%!T", \
                    0, /* Max filesize: -1 unlimited, -2 -don't change */\
                    ll_INFO); /* [ll_FATAL...ll_DEBUG] */
#else
    lolog = INIT_LOG();
#endif
    UL_TRACE((LOGID, "DllMain(process_attach) LightOPC.dll"));
   }
 else if (fdwReason == DLL_PROCESS_DETACH)
   {
    UL_TRACE((LOGID, "DllMain(process_detach) LightOPC.dll"));
    unilog_Delete(lolog); 
    unilog_Close(lolog); 
    lolog = 0;
   }

 return TRUE;
}




/***************************************************************************/
