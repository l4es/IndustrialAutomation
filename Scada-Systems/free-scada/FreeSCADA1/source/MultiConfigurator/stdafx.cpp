// stdafx.cpp : source file that includes just the standard includes
//	MultyConfigurator.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

//#include "../3rdParty/opc/opcda_i.c"
#include "../3rdParty/opc/OpcEnum_i.c"

	CString BrowseComputer(HWND hParent)
	{
		CString result;
		BROWSEINFO bi;
		LPITEMIDLIST	proot,pil;
		proot = SHCloneSpecialIDList(hParent, CSIDL_NETWORK, FALSE);
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner	= hParent;
		bi.ulFlags		= BIF_BROWSEFORCOMPUTER;
		bi.pidlRoot		= proot;
		pil = SHBrowseForFolder(&bi);
		if(pil)
		{
			IShellFolder *isf;
			SHGetDesktopFolder(&isf);
			STRRET cn;
			ZeroMemory(&cn, sizeof(STRRET));
			isf->GetDisplayNameOf(pil,SHGDN_FORPARSING, &cn);
			switch(cn.uType)
			{
			case STRRET_CSTR:
				result = cn.cStr;
				break;
			case STRRET_WSTR:
				result = cn.pOleStr;
				break;
			case STRRET_OFFSET:
				result = CString((byte*)pil + cn.uOffset);
				break;
			}
			isf->Release();
			ILFree(pil);
		}
		ILFree(proot);
		return result;
	}