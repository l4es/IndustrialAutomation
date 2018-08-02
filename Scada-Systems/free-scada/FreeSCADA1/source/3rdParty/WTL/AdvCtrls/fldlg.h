#ifndef __FLDLG__H
#define __FLDLG__H

#pragma once

/////////////////////////////////////////////////////
//	Advanced OpenFileDialog
//
//	Written by Alexey Shirshov (qqqaqa@mail.ru)
//	Copyright (c) 2003 Alexey Shirshov
//
//
//
//

#ifndef __ATLDLGS_H__
#error flgdlg.h requires atldlgs.h to be included first
#endif

//Need for SHDeleteKey
#if (_WIN32_WINNT >= 0x0500)
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi")
#endif

namespace AWTL
{
class CFileDialogEx : public CFileDialogImpl<CFileDialogEx>
{
	typedef CFileDialogImpl<CFileDialogEx> _baseClass;
public:
	CFileDialogEx(bool fOpen = true,LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,HWND hWndParent = NULL):
		_baseClass(fOpen,lpszDefExt,lpszFileName,dwFlags,
			lpszFilter,hWndParent)
	{
		fShowPlaceBar = true;
	}

	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		ATLASSERT(m_ofn.Flags & OFN_ENABLEHOOK);
		ATLASSERT(m_ofn.lpfnHook != NULL);	// can still be a user hook

		ATLASSERT(m_ofn.Flags & OFN_EXPLORER);

		if(m_ofn.hwndOwner == NULL)		// set only if not specified before
			m_ofn.hwndOwner = hWndParent;

		ATLASSERT(m_hWnd == NULL);
		_Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBase*)this);

		BOOL bRet;

#if (_WIN32_WINNT >= 0x0500)
		if (fShowPlaceBar && !AtlIsOldWindows()){			
			m_ofn.FlagsEx = 0;
			if (_places.GetSize() > 0){
				ATLASSERT(_places.GetSize() < 6);
				CRegKey reg;
				if (reg.Create(HKEY_CURRENT_USER, _T("AWTL")) == ERROR_SUCCESS){
					LONG lErr = ::RegOverridePredefKey(HKEY_CURRENT_USER,reg);
					ATLASSERT(lErr == ERROR_SUCCESS);
					if (lErr == ERROR_SUCCESS){
						CRegKey places;
						lErr = places.Create(HKEY_CURRENT_USER,
							_T("Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\comdlg32\\PlacesBar"));
						if (lErr == ERROR_SUCCESS){
							for(int i = 0;i < _places.GetSize();i++){
								const UINT iPlace = PtrToUint(_places[i]._pStr);
								ATLASSERT(iPlace <= 20 || iPlace > 0x0FFFF);

								TCHAR buf[100];
								wsprintf(buf,_T("Place%d"),i);
								
								if (iPlace <= 20)
									places.SetValue(iPlace,buf);
								else
									places.SetValue(_places[i]._pStr,buf);
							}
						}
					}
				}
			}
		}
#endif		
		if (m_bOpenFileDialog)
			bRet = ::GetOpenFileName(&m_ofn);
		else
			bRet = ::GetSaveFileName(&m_ofn);

#if (_WIN32_WINNT >= 0x0500)
		if (fShowPlaceBar && !AtlIsOldWindows() && _places.GetSize() > 0){
			LONG lErr = ::RegOverridePredefKey(HKEY_CURRENT_USER,NULL);
			ATLASSERT(lErr == ERROR_SUCCESS);
			SHDeleteKey(HKEY_CURRENT_USER,_T("AWTL"));
		}
#endif		
		m_hWnd = NULL;

		return bRet ? IDOK : IDCANCEL;
	}

	bool SetShowPlaceBar(bool b = true)
	{
		bool t = fShowPlaceBar;
		fShowPlaceBar = b;
		return t;
	}

	bool Add(int iPlace)
	{		
		return _places.Add(_place_holder(iPlace)) == TRUE;
	}

	bool Add(PCTSTR pPlace)
	{
		return _places.Add(_place_holder(pPlace)) == TRUE;
	}

	bool Remove(PCTSTR pPlace)
	{
		return _places.Remove(_place_holder(pPlace)) == TRUE;
	}

	bool Remove(int iPlace)
	{
		return _places.Remove(_place_holder(iPlace)) == TRUE;
	}

private:
	union _place_holder
	{
		_place_holder(PCTSTR pStr)
		{
			_pStr = pStr;
			//_iPlace = -1;
		}
		_place_holder(int iPlace)
		{
			_pStr = (PCTSTR)IntToPtr(iPlace);
			//_pStr = NULL;
		}

		//Need for Remove by value
		//(that calls Find, that required operator==)
		bool operator==(const _place_holder& _ph)
		{
			const UINT iPlace = PtrToUint(_pStr);
			const UINT _iPlace = PtrToUint(_ph._pStr);

			if (iPlace > 0x0FFFF && _iPlace > 0x0FFFF)
				return lstrcmpi(_pStr,_ph._pStr) == 0;
			else if (iPlace <= 20 && _iPlace <= 20)
				return iPlace == _iPlace;
			else
				return false;
		}
		
		PCTSTR _pStr;
	};
	bool fShowPlaceBar;
	CSimpleValArray<_place_holder> _places;

public:
	enum{
		Desktop = 0,StartMenuPrograms = 2,ControlPanel = 3,
		Printers = 4,MyDocuments = 5,Favorites = 6,
		ProgramsStartup = 7,RecentFiles = 8,sendto = 9,
		Recycle = 10,StartMenu = 12,MyComputer = 17,
		NetworkPlaces = 18,Fonts = 20
	};
};	//end of CFileDialogEx

}	//end of AWTL namespace

#endif	//__FLDLG__H