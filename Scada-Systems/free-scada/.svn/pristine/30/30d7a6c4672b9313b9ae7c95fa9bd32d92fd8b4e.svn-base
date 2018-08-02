#include "stdafx.h"
#include "zlibcpp.h"
#include "utility.h"

CZLib::CZLib()
{
	m_zf = 0;
}

CZLib::~CZLib()
{
	if (m_zf)
		Close();
}

BOOL CZLib::Open(std::wstring f_file, int f_nAppend)
{
	m_zf = zipOpen(hl::conv_wstr_to_str(f_file).c_str(), f_nAppend);
	return (m_zf != NULL);
}

void CZLib::Close()
{
	if (m_zf)
		zipClose(m_zf, NULL);

	m_zf = 0;
}

BOOL CZLib::AddFile(std::wstring f_file)
{
	BOOL bReturn = FALSE;

	// Open file being added
	HANDLE hFile = NULL;
	hFile = CreateFile(f_file.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		// Get file creation date
		FILETIME       ft = crash_reporter::CUtility::getLastWriteFileTime(f_file);
		zip_fileinfo   zi = {0};

		FileTimeToDosDateTime(
			&ft,                       // last write FILETIME
			((LPWORD)&zi.dosDate)+1,   // dos date
			((LPWORD)&zi.dosDate)+0);  // dos time

		// Trim path off file name
		std::wstring sFileName(f_file.begin()+f_file.find_last_of(L'\\')+1,f_file.end());

		// Start a new file in Zip
		if (ZIP_OK == zipOpenNewFileInZip(m_zf,
			hl::conv_wstr_to_str(sFileName).c_str(),
			&zi, 
			NULL, 
			0, 
			NULL, 
			0, 
			NULL, 
			Z_DEFLATED, 
			Z_BEST_COMPRESSION))
		{
			// Write file to Zip in 4 KB chunks 
			const DWORD BUFFSIZE    = 4096;
			TCHAR buffer[BUFFSIZE]  = _T("");
			DWORD dwBytesRead       = 0;

			while (ReadFile(hFile, &buffer, BUFFSIZE, &dwBytesRead, NULL)
				&& dwBytesRead)
			{
				if (ZIP_OK == zipWriteInFileInZip(m_zf, buffer, dwBytesRead)
					&& dwBytesRead < BUFFSIZE)
				{
					// Success
					bReturn = TRUE;
				}
			}

			bReturn &= (ZIP_OK == zipCloseFileInZip(m_zf));
		}

		bReturn &= CloseHandle(hFile);
	}

	return bReturn;
}