#pragma once

namespace crash_reporter
{
	class CUtility  
	{
	public:

		//-----------------------------------------------------------------------------
		// getLastWriteFileTime
		//    Returns the time the file was last modified in a FILETIME structure.
		//
		// Parameters
		//    sFile       Fully qualified file name
		//
		// Return Values
		//    FILETIME structure
		//
		// Remarks
		//
		static FILETIME getLastWriteFileTime(std::wstring sFile);

		//-----------------------------------------------------------------------------
		// getAppName
		//    Returns the application module's file name
		//
		// Parameters
		//    none
		//
		// Return Values
		//    File name of the executable
		//
		// Remarks
		//    none
		//
		static std::wstring getAppName();

		//-----------------------------------------------------------------------------
		// getSaveFileName
		//    Presents the user with a save as dialog and returns the name selected.
		//
		// Parameters
		//    none
		//
		// Return Values
		//    Name of the file to save to, or "" if the user cancels.
		//
		// Remarks
		//    none
		//
		static std::wstring getSaveFileName();

		//-----------------------------------------------------------------------------
		// getTempFileName
		//    Returns a generated temporary file name
		//
		// Parameters
		//    none
		//
		// Return Values
		//    Temporary file name
		//
		// Remarks
		//
		static std::wstring getTempFileName();
	};
}
