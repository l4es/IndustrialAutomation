#include "common.h"
#include ".\filemanager.h"


CFileManager::CFileManager(void)
{
}

CFileManager::~CFileManager(void)
{
}

UINT CFileManager::NewImage(CString FileName, CString ImageName)
{
	FILE *file = _tfopen(FileName, _T("rb"));
	if(file == NULL)
		return E_INVALIDARG;

	FIBITMAP *dib = NULL;

	CString ext,fn;
	int pos = 0;
	do 
	{
		pos = FileName.Find(_T('.'),pos+1);
	} while(FileName.Find(_T('.'),pos+1) >= 0);

	ext = FileName.Mid(pos+1);
	fn = (CString)FileName;

	FreeImage_Initialise();

	switch(FreeImage_GetFileTypeFromExt((LPTSTR)(LPCTSTR)ext)) {
				case FIF_BMP :
					dib = FreeImage_LoadBMP((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_ICO :
					dib = FreeImage_LoadICO((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_JPEG :
					dib = FreeImage_LoadJPEG((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_KOALA :
					dib = FreeImage_LoadKOALA((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PCD :
					dib = FreeImage_LoadPCD((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PCX :
					dib = FreeImage_LoadPCX((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PNG :
					dib = FreeImage_LoadPNG((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PBM :
					dib = FreeImage_LoadPNM((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PGM :
					dib = FreeImage_LoadPNM((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_PPM :
					dib = FreeImage_LoadPNM((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_RAS :
					dib = FreeImage_LoadRAS((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_TARGA :
					dib = FreeImage_LoadTARGA((LPTSTR)(LPCTSTR)fn);
					break;

				case FIF_TIFF :
					dib = FreeImage_LoadTIFF((LPTSTR)(LPCTSTR)fn);
					break;
	};
	if(dib == NULL)
	{
	//	ATLTRACE("Ошибка загрузки файла\n");
		return 0;
	}

	FIBITMAP* ptr = FreeImage_ConvertTo32Bits(dib);
	FreeImage_Free(dib);
	dib = ptr;

	sImage image;
	image.Width		= FreeImage_GetWidth(dib);
	image.Height	= FreeImage_GetHeight(dib);
	image.RedMask	= FreeImage_GetRedMask(dib);
	image.GreenMask	= FreeImage_GetGreenMask(dib);
	image.BlueMask	= FreeImage_GetBlueMask(dib);
	image.data = new COLORREF[image.Width*image.Height];

	memcpy(	image.data, 
		FreeImage_GetBits(dib),
		image.Width*image.Height*sizeof(COLORREF));

	FreeImage_Free(dib);
	FreeImage_DeInitialise();

	m_Names.push_back(ImageName);
	m_Images.push_back(image);

	return 0;
}

