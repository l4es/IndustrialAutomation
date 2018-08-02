#include "StdAfx.h"
#include "CfgXml.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		HelpersLib::InitializeStandardCfgHandlers();
		break;

	case DLL_PROCESS_DETACH:
		HelpersLib::CleanupCfgHandlers();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}