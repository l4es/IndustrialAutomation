// TestCaseMBReadHolding.h: interface for the CTestCaseMBReadHolding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTCASEMBREADHOLDING_H__255134B2_3945_4F5C_B2E6_F9A526A92E74__INCLUDED_)
#define AFX_TESTCASEMBREADHOLDING_H__255134B2_3945_4F5C_B2E6_F9A526A92E74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TestSvr.h"


#define MBFUNCTION_READHOLDING            0x03
#define MBFUNCTION_PRESETSINGLEHOLDING    0x06
#define MBFUNCTION_PRESETMULTIPLEHOLDING  0x10
#define MBFUNCTION_MASKWRITEHOLDING       0x16
#define MBFUNCTION_WRITE_MULTIPLE_COILS   0x0F

#define MB_FUNCTION_USER                  0x42



///////////////////////////////////////////////////////////////////
class CTestCaseMBBase : public CTestCase  
{
public:


   CTestCaseMBBase(LPCTSTR name, LPCTSTR description);

   bool CRCValid( CProtFrame &frame);
   bool StepReadSingleHolding(WORD registerStart, WORD &value);
   bool StepWriteSingleHolding(WORD registerStart, WORD value);


protected:
   bool OpenChannel();
   int CloseChannel();


private:
   CTestCaseMBBase();


protected:
   //data
   CModbusRTURoot    portA;
   CPortSettingBase  set;
};


///////////////////////////////////////////////////////////////////
class CTestCaseMBReadHolding : public CTestCaseMBBase
{
public:
	CTestCaseMBReadHolding();
	virtual ~CTestCaseMBReadHolding();

   // overloads:
   virtual bool Run();

   virtual void BuildFrame()  {};
   virtual void SendFrame() {};
   virtual void RecieveFrame() {};
   virtual void TestFrame(bool &aLast)  {Run(); aLast=TRUE;};

};


///////////////////////////////////////////////////////////////////
class CTestCaseMBWriteHolding : public CTestCaseMBBase  
{
public:
	CTestCaseMBWriteHolding();
	virtual ~CTestCaseMBWriteHolding();

   virtual void BuildFrame()  {};
   virtual void SendFrame() {};
   virtual void RecieveFrame() {};
   virtual void TestFrame(bool &aLast)  {Run(); aLast=TRUE;};


   // overloads:
   virtual bool Run();
};


///////////////////////////////////////////////////////////////////
class CTestCaseMBPresetSingleHolding : public CTestCaseMBBase  
{
public:
	CTestCaseMBPresetSingleHolding();
	virtual ~CTestCaseMBPresetSingleHolding();

   virtual void BuildFrame()  {};
   virtual void SendFrame() {};
   virtual void RecieveFrame() {};
   virtual void TestFrame(bool &aLast)  {Run(); aLast=TRUE;};


   // overloads:
   virtual bool Run();
};


///////////////////////////////////////////////////////////////////
class CTestCaseMBMaskWriteHolding : public CTestCaseMBBase  
{
public:
	CTestCaseMBMaskWriteHolding();
	virtual ~CTestCaseMBMaskWriteHolding();

   virtual void BuildFrame()  {};
   virtual void SendFrame() {};
   virtual void RecieveFrame() {};
   virtual void TestFrame(bool &aLast)  {Run(); aLast=TRUE;};


   bool StepMaskWriteHolding(WORD registerStart, WORD andMask, WORD orMask);
   // overloads:
   virtual bool Run();

private:
   //CModbusRTURoot portA;

};





#endif // !defined(AFX_TESTCASEMBREADHOLDING_H__255134B2_3945_4F5C_B2E6_F9A526A92E74__INCLUDED_)
