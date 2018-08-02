#include "StdAfx.h"
#include ".\limitsview.h"

void CLimitsView::OnInitialUpdate()
{
	m_GraphType = (int)((CDocument*)GetDocument())->m_GraphicType;

	SetWindowText(_T("Dialog View"));
	DlgResize_Init(false, true, NULL);
	DoDataExchange(DDX_LOAD);

	CDocument* pDoc = (CDocument*)GetDocument();

	CDateTimePickerCtrl dt = GetDlgItem(IDC_FROM_DATE);
	SYSTEMTIME st;
	pDoc->m_FromDateTime.GetAsSystemTime(st);
	dt.SetSystemTime(GDT_VALID, &st);
	dt = GetDlgItem(IDC_FROM_TIME);
	dt.SetSystemTime(GDT_VALID, &st);

	dt = GetDlgItem(IDC_TO_DATE);
	pDoc->m_ToDateTime.GetAsSystemTime(st);
	dt.SetSystemTime(GDT_VALID, &st);
	dt = GetDlgItem(IDC_TO_TIME);
	dt.SetSystemTime(GDT_VALID, &st);
}

BOOL CLimitsView::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

void CLimitsView::OnUpdate()
{
}

void CLimitsView::EnableStaticGroup(BOOL Enable)
{
	GetDlgItem(IDC_STATIC_GROUP).EnableWindow(Enable);
	GetDlgItem(IDC_FROM_DATE).EnableWindow(Enable);
	GetDlgItem(IDC_FROM_TIME).EnableWindow(Enable);
	GetDlgItem(IDC_TO_DATE).EnableWindow(Enable);
	GetDlgItem(IDC_TO_TIME).EnableWindow(Enable);
}

LRESULT CLimitsView::OnDateTimeChange(int /*idCtrl*/, LPNMHDR /*pNMHDR*/, BOOL& /*bHandled*/)
{
	CDocument* pDoc = (CDocument*)GetDocument();

	CDateTimePickerCtrl dt;
	SYSTEMTIME st,tmp;

	dt = GetDlgItem(IDC_FROM_DATE);
	dt.GetSystemTime(&st);
	tmp = st;
	dt = GetDlgItem(IDC_FROM_TIME);
	dt.GetSystemTime(&st);
	tmp.wHour	= st.wHour;
	tmp.wMinute	= st.wMinute;
	tmp.wSecond	= st.wSecond;
	pDoc->m_FromDateTime = tmp;

	dt = GetDlgItem(IDC_TO_DATE);
	dt.GetSystemTime(&st);
	tmp = st;
	dt = GetDlgItem(IDC_TO_TIME);
	dt.GetSystemTime(&st);
	tmp.wHour	= st.wHour;
	tmp.wMinute	= st.wMinute;
	tmp.wSecond	= st.wSecond;
	pDoc->m_ToDateTime = tmp;
	
//	pDoc->UpdateAllViews(this);

	return 0;
}
LRESULT CLimitsView::OnUpdateBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	GetDocument()->UpdateAllViews();
	return 0;
}
