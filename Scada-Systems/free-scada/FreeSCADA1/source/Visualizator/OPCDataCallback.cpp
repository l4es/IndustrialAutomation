#include "stdafx.h"
#include "OPCDataCallback.h"
#include "MainFrm.h"
#include "doc.h"

STDMETHODIMP COPCDataCallback::OnDataChange(
						  DWORD       dwTransid, 
						  OPCHANDLE   hGroup, 
						  HRESULT     hrMasterquality,
						  HRESULT     hrMastererror,
						  DWORD       dwCount, 
						  OPCHANDLE * phClientItems, 
						  VARIANT   * pvValues, 
						  WORD      * pwQualities,
						  FILETIME  * pftTimeStamps,
						  HRESULT   * pErrors
						  )
{
	ATLASSERT(m_Doc != NULL);

	//Метод вызывается OPC сервером при обновлении данных

	CVariableMenagerPtr mngr;

	//ATLTRACE("** count=%u\n",dwCount);
	for(ULONG i=0;i<dwCount;i++)
	{
		//ATLTRACE("** \ttag=%u\n",phClientItems[i]);
		//m_Doc->UpdateTagTimer(phClientItems[i]);

		//Если "качество" значение отличается от хорошего
		//пропускаем его
		if(!(pwQualities[i] & Q_GOOD))
			continue;
		
		//Ищем имя переменной для которой пришли данные (по хэндлу)
		wstring VarName = m_Doc->GetVariableNameByOPCHandle(phClientItems[i]);
		//Записать новое значение (не передавать его на OPC)
		mngr->SetVariable(VarName, pvValues[i], false);
		
		//Вывод информации в строку статуса о последней измененной переменной
		if(g_MainFrame && g_MainFrame->IsWindow())
		{
			//Получить строку статуса
			CStatusBarCtrl bar = g_MainFrame->m_hWndStatusBar;
			if(bar.IsWindow())
			{
				CComVariant v(pvValues[i]);
				SYSTEMTIME st;
				ATL::CString fmt;
				
				//Получить время, во сколько пришли данные 
				//(перевод к локальному времени с учетом GMT)
				FileTimeToLocalFileTime(&pftTimeStamps[i], &pftTimeStamps[i]);
				FileTimeToSystemTime(&pftTimeStamps[i], &st);

				if(v.vt == VT_BOOL)
					v = v.boolVal?1:0;

				if(v.vt == VT_R4 || v.vt == VT_R8)
				{
					//Для вещественных типов использовать принудительный формат
					v.ChangeType(VT_R8);
					fmt.Format(L"[%02u:%02u:%02u:%03u] Variable «%s» = %.3f", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, VarName.c_str(), v.dblVal);
				}
				else
				{
					v.ChangeType(VT_BSTR);
					fmt.Format(L"[%02u:%02u:%02u:%03u] Variable «%s» = %s", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, VarName.c_str(), ATL::CString(v.bstrVal));
				}
				
				bar.SetWindowText(fmt);
			}
		}
	}
	return S_OK;
}

STDMETHODIMP COPCDataCallback::OnReadComplete(
							DWORD       dwTransid, 
							OPCHANDLE   hGroup, 
							HRESULT     hrMasterquality,
							HRESULT     hrMastererror,
							DWORD       dwCount, 
							OPCHANDLE * phClientItems, 
							VARIANT   * pvValues, 
							WORD      * pwQualities,
							FILETIME  * pftTimeStamps,
							HRESULT   * pErrors
							)
{
	return S_OK;
}

STDMETHODIMP COPCDataCallback::OnWriteComplete(
							 DWORD       dwTransid, 
							 OPCHANDLE   hGroup, 
							 HRESULT     hrMastererr, 
							 DWORD       dwCount, 
							 OPCHANDLE * pClienthandles, 
							 HRESULT   * pErrors
							 )
{
	return S_OK;
}

STDMETHODIMP COPCDataCallback::OnCancelComplete(
							  DWORD       dwTransid, 
							  OPCHANDLE   hGroup
							  )
{
	return S_OK;
}