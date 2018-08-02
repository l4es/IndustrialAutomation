#if !defined(AFX_MYHOSTPROXY_H__476FB980_9F8F_40F5_9D31_FCB4AE615943__INCLUDED_)
#define AFX_MYHOSTPROXY_H__476FB980_9F8F_40F5_9D31_FCB4AE615943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyHostProxy.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CMyHostProxy command target

class CMyHostProxy : public CActiveScriptHost
{
	DECLARE_DYNCREATE(CMyHostProxy)

	CMyHostProxy();           // protected constructor used by dynamic creation
	virtual ~CMyHostProxy();

   virtual BOOL GetDispatchIID(IID* riid);
	DECLARE_OLETYPELIB(CMyHostProxy)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyHostProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMyHostProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMyHostProxy)
	afx_msg short xGetRegisterValue(long registertype, long registernumber);
	afx_msg void xEnableStation(short stationid);
	afx_msg void xDisableStation(short stationid);
	afx_msg void xDisplayAbout();
	afx_msg long xGetLastRunTime();
	afx_msg void xSetRegisterValue(long registertype, long registernumber, long registervalue);
	afx_msg long xStationEnabled(short stationid);
	afx_msg void xClearRegisters();
	afx_msg long xTransmitRegisters(short sourceStation, short destinationStation, long file_areaNumber, long startRegister, long numRegisters);
	afx_msg void xAddDebugString(LPCTSTR text);
	afx_msg long xMasterBusy();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYHOSTPROXY_H__476FB980_9F8F_40F5_9D31_FCB4AE615943__INCLUDED_)
