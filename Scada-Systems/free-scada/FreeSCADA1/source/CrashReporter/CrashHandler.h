#pragma once

#include "CrashReporter.h"
#include "ReportInfo.h"       // bulk of crash report generation

namespace crash_reporter
{
	class CCrashHandler  
	{
	public:

		//-----------------------------------------------------------------------------
		// CCrashHandler
		//    Initializes the library and optionally set the client crash callback and
		//    sets up the email details.
		//
		// Parameters
		//    lpfn        Client crash callback
		//    lpcszTo     Email address to send crash report
		//    lpczSubject Subject line to be used with email
		//
		// Return Values
		//    none
		//
		// Remarks
		//    Passing NULL for lpTo will disable the email feature and cause the crash 
		//    report to be saved to disk.
		//
		CCrashHandler(
			LPGETLOGFILE lpfn = NULL           // Client crash callback
			);

		//-----------------------------------------------------------------------------
		// ~CCrashHandler
		//    Uninitializes the crashrpt library.
		//
		// Parameters
		//    none
		//
		// Return Values
		//    none
		//
		// Remarks
		//    none
		//
		virtual 
			~CCrashHandler();

		//-----------------------------------------------------------------------------
		// AddFile
		//    Adds a file to the crash report.
		//
		// Parameters
		//    lpFile      Fully qualified file name
		//    lpDesc      File description
		//
		// Return Values
		//    none
		//
		// Remarks
		//    Call this function to include application specific file(s) in the crash
		//    report.  For example, applicatoin logs, initialization files, etc.
		//
		void 
			AddFile(
			LPCTSTR lpFile,                     // File nae
			LPCTSTR lpDesc                      // File description
			);

		//-----------------------------------------------------------------------------
		// GenerateErrorReport
		//    Produces a crash report.
		//
		// Parameters
		//    pExInfo     Pointer to an EXCEPTION_POINTERS structure
		//
		// Return Values
		//    none
		//
		// Remarks
		//    Call this function to manually generate a crash report.
		//
		void 
			GenerateErrorReport(
			PEXCEPTION_POINTERS pExInfo         // Exception pointers (see MSDN)
			);

	protected:

		//-----------------------------------------------------------------------------
		// SaveReport
		//    Presents the user with a file save dialog and saves the crash report
		//    file to disk.  This function is called if an Email:To was not provided
		//    in the constructor.
		//
		// Parameters
		//    rpt         The report details
		//    lpcszFile   The zipped crash report
		//
		// Return Values
		//    True is successful.
		//
		// Remarks
		//    none
		//
		BOOL 
			SaveReport(
			CReportInfo &rpt, 
			LPCTSTR lpcszFile
			);

		LPTOP_LEVEL_EXCEPTION_FILTER  m_oldFilter;      // previous exception filter
		LPGETLOGFILE                  m_lpfnCallback;   // client crash callback
		int                           m_pid;            // process id
		string_map_t                  m_files;          // custom files to add
	};
}
