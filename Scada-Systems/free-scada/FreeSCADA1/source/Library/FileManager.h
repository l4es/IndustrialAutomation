#pragma once

class CFileManager
{
	struct sImage
	{
		WORD		Width;
		WORD		Height;
		COLORREF*	data;
		DWORD		RedMask,
			GreenMask,
			BlueMask;
	};
public:
	CFileManager(void);
	~CFileManager(void);

	UINT NewImage(CString FileName, CString ImageName);

	vector<CString> m_Names;
	vector<sImage> m_Images;
};
