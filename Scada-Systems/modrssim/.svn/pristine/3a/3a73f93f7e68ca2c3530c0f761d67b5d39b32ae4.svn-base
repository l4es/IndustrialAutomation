
#if !defined(AFX_TESTSVR_H__6D68E757_7DA7_4301_A87C_9B9AF1961207__INCLUDED_)
#define AFX_TESTSVR_H__6D68E757_7DA7_4301_A87C_9B9AF1961207__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <conio.h>
#include "mtransportprovider.h"

#define MAX_DEBUG_STR_LEN  256

#define CHECKRESULT(a,b) CheckResult(a,b, __LINE__, __FILE__)

#define  HORIZ_DIVIDER ("------------------------------------------------------------------------------")

// TODO: implement a text-file logging system
class CTestLogger
{
public:

};


// test case base-class
class CTestCase
{
public:
   // TYPES
   enum ETestResult {EFail , EPass, ENotRun=-1, EUnknown=-2 };

   // METHODS
   CTestCase(LPCTSTR name, LPCTSTR description);

   ETestResult TestResult() { return(m_result); };  // noun for GetResult
   LPCTSTR TestResultName();

   void SetTestResult(const ETestResult res) {m_result = res;};
   bool Run();


   virtual void BuildFrame() = 0;
   virtual void SendFrame()= 0;
   virtual void RecieveFrame()= 0;
   virtual void TestFrame(bool &aLast) = 0;


   void CheckResult(bool result, LPCTSTR errorCause, int lineNum, LPCTSTR srcFileName);

private:   
   CTestCase() { m_result = ENotRun;}
   

   // DATA
public:

   CString m_name;
   CString m_description;

private:
   ETestResult m_result;

private:

};


// test-server :the test-runner 
class CTestServer
{

public:
   bool Execute (CTestCase *pTest);

};


class CTestUtilities
{

};

#endif // !defined(AFX_TESTSVR_H__6D68E757_7DA7_4301_A87C_9B9AF1961207__INCLUDED_)
