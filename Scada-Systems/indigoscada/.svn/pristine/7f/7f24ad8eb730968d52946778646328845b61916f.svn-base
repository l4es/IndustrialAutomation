#ifndef __RIPC_DEF_H__
#define __RIPC_DEF_H__

#include <stddef.h>

#ifdef _WIN32
#pragma  warning(disable:4250)
#endif

#ifdef RIPC_DLL
#ifdef RIPC_IMPLEMENTATION
#define RIPC_DLL_ENTRY __declspec(dllexport)
#else
#define RIPC_DLL_ENTRY __declspec(dllimport)
#endif
#else
#define RIPC_DLL_ENTRY
#endif

#endif
