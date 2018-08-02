// TestCaseLoopbk.h: interface for the CTestCaseLoopbk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTCASELOOPBK_H__02CBB07B_2F96_4222_BD90_F9D693DD5F8A__INCLUDED_)
#define AFX_TESTCASELOOPBK_H__02CBB07B_2F96_4222_BD90_F9D693DD5F8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "TestSvr.h"

class CTestCaseLoopbk : public CTestCase  
{
public:
	CTestCaseLoopbk();
	virtual ~CTestCaseLoopbk();


   virtual void BuildFrame()  {};
   virtual void SendFrame() {};
   virtual void RecieveFrame() {};
   virtual void TestFrame(bool &aLast)  {aLast=TRUE;};


   // overloads:
   virtual bool Run();

private:

   CModbusRTURoot portA;
   CModbusRTURoot portB;

};

#endif // !defined(AFX_TESTCASELOOPBK_H__02CBB07B_2F96_4222_BD90_F9D693DD5F8A__INCLUDED_)
