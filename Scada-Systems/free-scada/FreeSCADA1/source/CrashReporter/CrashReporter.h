#pragma once

#include "exports.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace crash_reporter
{
	// Client crash callback
	typedef BOOL (CALLBACK *LPGETLOGFILE) (LPVOID lpvState);

	//-----------------------------------------------------------------------------
	// Install
	//    Initializes the library and optionally set the client crash callback and
	//    sets up the email details.
	//
	// Parameters
	//    pfn         Client crash callback
	//
	// Return Values
	//    If the function succeeds, the return value is a pointer to the underlying
	//    crash object created.  This state information is required as the first
	//    parameter to all other crash report functions.
	//
	CRASHREPORTER_API LPVOID Install(LPGETLOGFILE pfn);

	//-----------------------------------------------------------------------------
	// Uninstall
	//    Uninstalls the unhandled exception filter set up in Install().
	//
	// Parameters
	//    lpState     State information returned from Install()
	//
	// Return Values
	//    void
	//
	// Remarks
	//    This call is optional.  The crash report library will automatically 
	//    deinitialize when the library is unloaded.  Call this function to
	//    unhook the exception filter manually.
	//
	CRASHREPORTER_API void Uninstall(LPVOID lpState);

	//-----------------------------------------------------------------------------
	// AddFile
	//    Adds a file to the crash report.
	//
	// Parameters
	//    lpState     State information returned from Install()
	//    lpFile      Fully qualified file name
	//    lpDesc      Description of file, used by details dialog
	//
	// Return Values
	//    void
	//
	// Remarks
	//    This function can be called anytime after Install() to add one or more
	//    files to the generated crash report.
	//
	CRASHREPORTER_API void AddFile(LPVOID lpState, LPCTSTR lpFile, LPCTSTR lpDesc);

	//-----------------------------------------------------------------------------
	// GenerateErrorReport
	//    Generates the crash report.
	//
	// Parameters
	//    lpState     State information returned from Install()
	//    pExInfo     Pointer to an EXCEPTION_POINTERS structure
	//
	// Return Values
	//    void
	//
	// Remarks
	//    Call this function to manually generate a crash report.
	//
	CRASHREPORTER_API void GenerateErrorReport(LPVOID lpState, PEXCEPTION_POINTERS pExInfo);
}